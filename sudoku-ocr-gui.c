#include <gtk/gtk.h>
#include <pthread.h>
#include "grid_processing.h"
#include "image.h"
#include "image_processing.h"
#include "perspective_correction.h"
#include "rotation.h"

#define IMAGE_WIDTH 500
#define IMAGE_HEIGHT 500
#define VERBOSE_MODE 0
#define VERBOSE_PATH NULL

struct StepIndicators
{
    int current_step;
    GtkWidget *step_1_indicator;
    GtkWidget *step_2_indicator;
    GtkWidget *step_3_indicator;
    GtkWidget *step_4_indicator;
    GtkWidget *step_5_indicator;
};
typedef struct StepIndicators StepIndicators;

struct Controls
{
    GtkScale *rotation_scale;
    GtkButton *save_button;
    GtkButton *open_button;
    GtkButton *prev_button;
    GtkButton *cancel_image_button;
    GtkButton *confirm_image_button;
    GtkButton *image_rotation_done_button;
    GtkFileChooserButton *file_chooser_button;
};
typedef struct Controls Controls;

struct Page
{
    GtkViewport *container;
    GtkImage *image;
    GtkLabel *label;
};
typedef struct Page Page;

struct Pages
{
    gchar *current_page;
    Page *page1;
    Page *page2;
    Page *page3;
    Page *page4;
};
typedef struct Pages Pages;

struct Images
{
    Image *clean;
    Image *mask;
    Image *image;
    Image *image_rotated;
    Image *image_rotated_clean;
    Image *image_rotated_cropped;
    gdouble current_rotation;
    gboolean rotated;
    square *grid_square;
};
typedef struct Images Images;

struct MainWindow
{
    GtkWidget *window;
    GtkStack *stack;
    StepIndicators *step_indicators;
    Controls *controls;
    Pages *pages;
    Images *images;
};
typedef struct MainWindow MainWindow;

void display_image(GtkImage *image_container, Image *image)
{
    GdkPixbuf *pixbuf = image_to_pixbuf(image);

    GdkRectangle allocation;

    double aspect_ratio = (double)image->width / (double)image->height;

    allocation.width = IMAGE_WIDTH;
    allocation.width *= aspect_ratio;
    allocation.height = IMAGE_HEIGHT;

    GdkPixbuf *resized = gdk_pixbuf_scale_simple(
        pixbuf, allocation.width, allocation.height, GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(image_container, resized);

    gdk_pixbuf_unref(pixbuf);
}

void set_page(MainWindow *main_window, const gchar *page)
{
    main_window->pages->current_page = page;

    gtk_stack_set_visible_child_name(main_window->stack, page);
}

void set_step(StepIndicators *step_indicators, int step_number)
{
    step_indicators->current_step = step_number;

    gtk_widget_set_opacity(step_indicators->step_1_indicator, 0.5);
    gtk_widget_set_opacity(step_indicators->step_2_indicator, 0.5);
    gtk_widget_set_opacity(step_indicators->step_3_indicator, 0.5);
    gtk_widget_set_opacity(step_indicators->step_4_indicator, 0.5);
    gtk_widget_set_opacity(step_indicators->step_5_indicator, 0.5);

    switch (step_number)
    {
        case 1:
            gtk_widget_set_opacity(step_indicators->step_1_indicator, 1.0);
            break;
        case 2:
            gtk_widget_set_opacity(step_indicators->step_2_indicator, 1.0);
            break;
        case 3:
            gtk_widget_set_opacity(step_indicators->step_3_indicator, 1.0);
            break;
        case 4:
            gtk_widget_set_opacity(step_indicators->step_4_indicator, 1.0);
            break;
        case 5:
            gtk_widget_set_opacity(step_indicators->step_5_indicator, 1.0);
            break;
    }
}

void save_current_image(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    gchar *page = main_window->pages->current_page;

    Image *image;
    switch (main_window->step_indicators->current_step)
    {
        case 1:
            return;
        case 2:
            if (main_window->images->rotated)
                image = main_window->images->image_rotated;
            else
                image = main_window->images->image;
            break;
        case 3:
            image = main_window->images->image_rotated;
            break;
        case 4:
            image = main_window->images->image_rotated_cropped;
            break;
        case 5:
            image = main_window->images->image_rotated_cropped;
            break;
        default:
            return;
    }

    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog
        = gtk_file_chooser_dialog_new("Save File", main_window->window, action,
            "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    gtk_file_chooser_set_current_name(chooser, "image.png");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        save_image(Image_to_SDL_Surface(image), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void previous_page(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    if (g_str_equal(main_window->pages->current_page, "page2"))
    {
        set_page(main_window, "page1");
        set_step(main_window->step_indicators, 1);
    }
    else if (g_str_equal(main_window->pages->current_page, "page3"))
    {
        set_page(main_window, "page2");
        set_step(main_window->step_indicators, 1);
    }
    else if (g_str_equal(main_window->pages->current_page, "page4"))
    {
        set_page(main_window, "page3");
        set_step(main_window->step_indicators, 2);
    }
}

void file_selected(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GtkFileChooser *file_chooser
        = GTK_FILE_CHOOSER(main_window->controls->file_chooser_button);

    GFile *file = gtk_file_chooser_get_file(file_chooser);
    char *path = g_file_get_path(file);

    *main_window->images->image = SDL_Surface_to_Image(load_image(path));
    *main_window->images->mask = SDL_Surface_to_Image(load_image(path));
    *main_window->images->clean = SDL_Surface_to_Image(load_image(path));

    display_image(
        main_window->pages->page2->image, main_window->images->clean);

    gchar label[100];
    g_snprintf(label, 100, "Image: %s", path);
    gtk_label_set_text(main_window->pages->page2->label, label);

    set_page(main_window, "page2");
}

void cancel_image_selection(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_page(main_window, "page1");
}

void manual_rotate_image(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    display_image(
        main_window->pages->page3->image, main_window->images->image);

    gchar label[100];
    g_snprintf(label, 100,
        "<span weight=\"bold\" size=\"large\">Rotating Image</span>");
    gtk_label_set_markup(main_window->pages->page3->label, label);

    set_step(main_window->step_indicators, 2);
    set_page(main_window, "page3");

    image_processing_extract_grid(main_window->images->mask,
        main_window->images->image, VERBOSE_MODE, VERBOSE_PATH);

    display_image(
        main_window->pages->page3->image, main_window->images->image);
}

void rotation_changed(GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *)user_data;

    gdouble value = gtk_range_get_value(
        GTK_RANGE(main_window->controls->rotation_scale));

    *main_window->images->image_rotated
        = rotate_image(main_window->images->image, value);

    main_window->images->rotated = TRUE;
    main_window->images->current_rotation = value;

    display_image(GTK_IMAGE(main_window->pages->page3->image),
        main_window->images->image_rotated);

    return;
}

void process_image(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    gchar label[100];
    g_snprintf(label, 100,
        "<span weight=\"bold\" size=\"large\">Processing Image</span>");
    gtk_label_set_markup(main_window->pages->page3->label, label);

    set_step(main_window->step_indicators, 3);

    if (!main_window->images->rotated)
    {
        *main_window->images->image_rotated
            = clone_image(main_window->images->image);
    }

    display_image(
        main_window->pages->page4->image, main_window->images->image_rotated);

    set_page(main_window, "page4");

    double rotation_amount = 0;
    main_window->images->grid_square
        = grid_processing_detect_grid(main_window->images->image_rotated,
            &rotation_amount, VERBOSE_MODE, VERBOSE_PATH);

    *main_window->images->image_rotated_clean
        = rotate_image(main_window->images->clean,
            rotation_amount + main_window->images->current_rotation);

    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated_clean);

    main_window->images->image_rotated_cropped
        = correct_perspective(main_window->images->image_rotated_clean,
            main_window->images->grid_square, VERBOSE_MODE, VERBOSE_PATH);

    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated_cropped);

    Image **cells = split_grid(main_window->images->image_rotated_cropped,
        VERBOSE_MODE, VERBOSE_PATH);

    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated_cropped);

    set_step(main_window->step_indicators, 4);
}

int main(int argc, char *argv[])
{
    gtk_init(NULL, NULL);

    GtkBuilder *builder = gtk_builder_new();

    if (gtk_builder_add_from_file(
            builder, "./graphical_interface/main.glade", NULL)
        == 0)
    {
        g_printerr("Error loading file\n");
        return 1;
    }

    // Retreve elements
    GtkWindow *window
        = GTK_WINDOW(gtk_builder_get_object(builder, "sudokuocr"));

    GtkButton *save_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "savebutton"));
    GtkButton *open_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "openbutton"));
    GtkButton *prev_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "prevbutton"));
    GtkButton *confirm_image_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "confirmimagebutton"));
    GtkButton *cancel_image_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "cancelimagebutton"));
    GtkButton *rotation_done_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "rotationdonebutton"));

    GtkFileChooserButton *file_chooser_button = GTK_FILE_CHOOSER_BUTTON(
        gtk_builder_get_object(builder, "fileselector"));

    GtkScale *rotation_scale
        = GTK_SCALE(gtk_builder_get_object(builder, "rotationscale"));

    GtkWidget *step_1_indicator
        = GTK_WIDGET(gtk_builder_get_object(builder, "step1indicator"));
    GtkWidget *step_2_indicator
        = GTK_WIDGET(gtk_builder_get_object(builder, "step2indicator"));
    GtkWidget *step_3_indicator
        = GTK_WIDGET(gtk_builder_get_object(builder, "step3indicator"));
    GtkWidget *step_4_indicator
        = GTK_WIDGET(gtk_builder_get_object(builder, "step4indicator"));
    GtkWidget *step_5_indicator
        = GTK_WIDGET(gtk_builder_get_object(builder, "step5indicator"));

    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack1"));

    GtkViewport *page1_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page1"));
    GtkViewport *page2_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page2"));
    GtkViewport *page3_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page3"));
    GtkViewport *page4_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page4"));

    GtkImage *page2_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "page2image"));
    GtkImage *page3_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "page3image"));
    GtkImage *page4_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "page4image"));

    GtkLabel *page2_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page2label"));
    GtkLabel *page3_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page3label"));
    GtkLabel *page4_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page4label"));

    StepIndicators step_indicators = {
        .current_step = 1,
        .step_1_indicator = step_1_indicator,
        .step_2_indicator = step_2_indicator,
        .step_3_indicator = step_3_indicator,
        .step_4_indicator = step_4_indicator,
        .step_5_indicator = step_5_indicator,
    };

    Controls controls = {
        .save_button = save_button,
        .open_button = open_button,
        .prev_button = prev_button,
        .file_chooser_button = file_chooser_button,
        .confirm_image_button = confirm_image_button,
        .cancel_image_button = cancel_image_button,
        .rotation_scale = rotation_scale,
        .image_rotation_done_button = rotation_done_button,
    };

    Page page1 = {
        .container = page1_container,
        .image = NULL,
        .label = NULL,
    };
    Page page2 = {
        .container = page2_container,
        .image = page2_image,
        .label = page2_label,
    };
    Page page3 = {
        .container = page3_container,
        .image = page3_image,
        .label = page3_label,
    };

    Page page4 = {
        .container = page4_container,
        .image = page4_image,
        .label = page4_label,
    };

    Pages pages = {
        .current_page = "page1",
        .page1 = &page1,
        .page2 = &page2,
        .page3 = &page3,
        .page4 = &page4,
    };

    Images images = {
        .rotated = FALSE,
        .current_rotation = 0,
        .mask = malloc(sizeof(Image)),
        .image = malloc(sizeof(Image)),
        .clean = malloc(sizeof(Image)),
        .image_rotated = malloc(sizeof(Image)),
        .image_rotated_clean = malloc(sizeof(Image)),
        .image_rotated_cropped = malloc(sizeof(Image)),
    };

    MainWindow main_window = {
        .window = window,
        .stack = stack,
        .step_indicators = &step_indicators,
        .controls = &controls,
        .pages = &pages,
        .images = &images,
    };

    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // File selection
    g_signal_connect(file_chooser_button, "selection-changed",
        G_CALLBACK(file_selected), &main_window);
    // Save dialog
    g_signal_connect(
        save_button, "clicked", G_CALLBACK(save_current_image), &main_window);

    // Previous page
    g_signal_connect(
        prev_button, "clicked", G_CALLBACK(previous_page), &main_window);

    // Confirm image
    g_signal_connect(confirm_image_button, "clicked",
        G_CALLBACK(manual_rotate_image), &main_window);
    // Cancel image
    g_signal_connect(cancel_image_button, "clicked",
        G_CALLBACK(cancel_image_selection), &main_window);

    // Rotation done
    g_signal_connect(rotation_done_button, "clicked",
        G_CALLBACK(process_image), &main_window);

    // Rotation Scale
    g_signal_connect(rotation_scale, "value-changed",
        G_CALLBACK(rotation_changed), &main_window);

    set_step(&step_indicators, 1);

    gtk_widget_show(GTK_WIDGET(window));

    gtk_main();

    return 0;
}