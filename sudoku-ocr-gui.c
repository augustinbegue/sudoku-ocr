#include <gtk/gtk.h>
#include <pthread.h>
#include "grid_processing.h"
#include "grid_splitting.h"
#include "image.h"
#include "image_processing.h"
#include "perspective_correction.h"
#include "result_network.h"
#include "rotation.h"

#define IMAGE_SIZE 500
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
    GtkButton *correction_done_button;
    GtkToggleButton *hide_grid_button;
    GtkToggleButton *hide_image_button;
    GtkFileChooserButton *file_chooser_button;
};
typedef struct Controls Controls;

struct Page
{
    GtkViewport *container;
    GtkDrawingArea *image;
    GtkLabel *label;
};
typedef struct Page Page;

struct Pages
{

    char *current_page;
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
    gboolean is_rotated;
    gboolean is_loaded;
    square *grid_square;
    gint display_size;
    GtkDrawingArea *current_container;
    Image *current_image;
};
typedef struct Images Images;

struct MainWindow
{
    GtkWidget *window;
    GtkStack *stack;
    GtkGrid *sudoku_labels_grid;
    StepIndicators *step_indicators;
    Controls *controls;
    Pages *pages;
    Images *images;
    int **grid;
};
typedef struct MainWindow MainWindow;

void draw_image_and_grid(
    GtkDrawingArea *drawing_area, cairo_t *cr, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GdkPixbuf *pixbuf = image_to_pixbuf(main_window->images->current_image);

    gint width = gtk_widget_get_allocated_width(GTK_WIDGET(drawing_area));
    gint height = gtk_widget_get_allocated_height(GTK_WIDGET(drawing_area));

    gint image_width = main_window->images->current_image->width;
    gint image_height = main_window->images->current_image->height;

    gdouble scale_x = (gdouble)width / image_width;
    gdouble scale_y = (gdouble)height / image_height;
    gdouble scale = MIN(scale_x, scale_y);

    gint scaled_width = image_width * scale;
    gint scaled_height = image_height * scale;
    gint scaled_x = (width - scaled_width) / 2;
    gint scaled_y = (height - scaled_height) / 2;

    pixbuf = gdk_pixbuf_scale_simple(
        pixbuf, scaled_width, scaled_height, GDK_INTERP_BILINEAR);

    cairo_surface_t *surface
        = gdk_cairo_surface_create_from_pixbuf(pixbuf, 1, NULL);

    cairo_set_source_surface(cr, surface, scaled_x, scaled_y);
    cairo_rectangle(cr, scaled_x, scaled_y, scaled_width, scaled_height);

    main_window->images->display_size
        = scaled_width > scaled_height ? scaled_height : scaled_width;

    gtk_widget_set_margin_start(
        GTK_WIDGET(main_window->sudoku_labels_grid), scaled_x + 20);
    gtk_widget_set_margin_top(
        GTK_WIDGET(main_window->sudoku_labels_grid), scaled_y + 20);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            GtkLabel *label = GTK_LABEL(gtk_grid_get_child_at(
                GTK_GRID(main_window->sudoku_labels_grid), i, j));

            gtk_widget_set_size_request(GTK_WIDGET(label),
                main_window->images->display_size / 9,
                main_window->images->display_size / 9);

            if (main_window->grid[i][j] != 0)
            {
                gchar label_text[100];

                g_snprintf(label_text, 100,
                    "<span font='%d' weight='bold' color='red'>%d</span>",
                    main_window->images->display_size / 18,
                    main_window->grid[i][j]);

                gtk_label_set_markup(label, label_text);
            }
            else
            {
                gtk_label_set_text(label, "");
            }
        }
    }

    cairo_fill(cr);

    g_object_unref(pixbuf);
}

void draw_image(GtkDrawingArea *drawing_area, cairo_t *cr, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GdkPixbuf *pixbuf = image_to_pixbuf(main_window->images->current_image);

    gint width = gtk_widget_get_allocated_width(GTK_WIDGET(drawing_area));
    gint height = gtk_widget_get_allocated_height(GTK_WIDGET(drawing_area));

    gint image_width = main_window->images->current_image->width;
    gint image_height = main_window->images->current_image->height;

    gdouble scale_x = (gdouble)width / image_width;
    gdouble scale_y = (gdouble)height / image_height;
    gdouble scale = MIN(scale_x, scale_y);

    gint scaled_width = image_width * scale;
    gint scaled_height = image_height * scale;
    gint scaled_x = (width - scaled_width) / 2;
    gint scaled_y = (height - scaled_height) / 2;

    pixbuf = gdk_pixbuf_scale_simple(
        pixbuf, scaled_width, scaled_height, GDK_INTERP_BILINEAR);

    cairo_surface_t *surface
        = gdk_cairo_surface_create_from_pixbuf(pixbuf, 1, NULL);

    cairo_set_source_surface(cr, surface, scaled_x, scaled_y);
    cairo_rectangle(cr, scaled_x, scaled_y, scaled_width, scaled_height);

    main_window->images->display_size
        = scaled_width > scaled_height ? scaled_height : scaled_width;

    cairo_fill(cr);

    g_object_unref(pixbuf);
}

void display_image(
    GtkDrawingArea *image_container, Image *image, MainWindow *main_window)
{
    main_window->images->current_container = image_container;

    if (main_window->images->display_size != 0)
        free_Image(main_window->images->current_image);
    main_window->images->display_size = IMAGE_SIZE;

    *main_window->images->current_image = clone_image(image);

    gtk_widget_queue_draw(GTK_WIDGET(image_container));
}

void set_page(MainWindow *main_window, gchar *page)
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

void set_button_to_load(GtkButton *button)
{
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

    GtkSpinner *spinner = GTK_SPINNER(gtk_spinner_new());
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(button));

    if (child)
        gtk_container_remove(GTK_CONTAINER(button), child);

    gtk_container_add(GTK_CONTAINER(button), GTK_WIDGET(spinner));

    gtk_widget_show(GTK_WIDGET(spinner));

    gtk_spinner_start(spinner);
}

void set_button_to_label(GtkButton *button, gchar *text)
{
    GtkLabel *label = GTK_LABEL(gtk_label_new(""));

    gtk_label_set_markup(label, text);

    GtkWidget *child = gtk_bin_get_child(GTK_BIN(button));

    if (child)
        gtk_container_remove(GTK_CONTAINER(button), child);

    gtk_container_add(GTK_CONTAINER(button), GTK_WIDGET(label));

    gtk_widget_show(GTK_WIDGET(label));

    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
}

void save_current_image(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    Image *image;
    switch (main_window->step_indicators->current_step)
    {
        case 1:
            return;
        case 2:
            if (main_window->images->is_rotated)
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

    dialog = gtk_file_chooser_dialog_new("Save File",
        GTK_WINDOW(main_window->window), action, "Cancel", GTK_RESPONSE_CANCEL,
        "Save", GTK_RESPONSE_ACCEPT, NULL);
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

void file_selected(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GtkFileChooser *file_chooser;

    if (GTK_IS_FILE_CHOOSER(widget))
        file_chooser = GTK_FILE_CHOOSER(widget);
    else
        file_chooser
            = GTK_FILE_CHOOSER(main_window->controls->file_chooser_button);

    GFile *file = gtk_file_chooser_get_file(file_chooser);
    char *path = g_file_get_path(file);

    *main_window->images->image = SDL_Surface_to_Image(load_image(path));
    *main_window->images->mask = SDL_Surface_to_Image(load_image(path));
    *main_window->images->clean = SDL_Surface_to_Image(load_image(path));
    main_window->images->is_loaded = TRUE;

    // Reset values
    main_window->images->current_rotation = 0;
    main_window->images->is_rotated = false;
    gtk_range_set_value(GTK_RANGE(main_window->controls->rotation_scale), 0);

    display_image(main_window->pages->page2->image, main_window->images->clean,
        main_window);

    gchar label[100];
    g_snprintf(label, 100, "Image: %s", path);
    gtk_label_set_text(main_window->pages->page2->label, label);

    if (widget != NULL)
        set_page(main_window, "page2");
}

gboolean grid_extraction_finished(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    display_image(main_window->pages->page3->image, main_window->images->image,
        main_window);

    set_button_to_label(main_window->controls->confirm_image_button,
        "<span weight=\"bold\">Use this image</span>");
    set_button_to_label(
        main_window->controls->image_rotation_done_button, "Done");
    gtk_widget_show(GTK_WIDGET(main_window->controls->rotation_scale));

    return FALSE;
}

void *grid_extraction_handler(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    image_processing_extract_grid(main_window->images->mask,
        main_window->images->image, VERBOSE_MODE, VERBOSE_PATH);

    g_idle_add(grid_extraction_finished, main_window);

    return NULL;
}

void manual_rotate_image(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    g_thread_new("grid_extraction_handler",
        (GThreadFunc)grid_extraction_handler, main_window);

    gtk_widget_hide(GTK_WIDGET(main_window->controls->rotation_scale));
    set_button_to_load(main_window->controls->confirm_image_button);
    set_button_to_load(main_window->controls->image_rotation_done_button);

    display_image(main_window->pages->page3->image, main_window->images->image,
        main_window);

    gchar label[100];
    g_snprintf(label, 100,
        "<span weight=\"bold\" size=\"large\">Rotating Image</span>");
    gtk_label_set_markup(main_window->pages->page3->label, label);

    set_step(main_window->step_indicators, 2);

    if (widget != NULL)
        set_page(main_window, "page3");
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
        set_step(main_window->step_indicators, 1);
        file_selected(NULL, main_window);
        set_page(main_window, "page2");
    }
    else if (g_str_equal(main_window->pages->current_page, "page4"))
    {
        file_selected(NULL, main_window);
        manual_rotate_image(NULL, main_window);
        set_page(main_window, "page3");
    }
}

void open_image(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
        GTK_WINDOW(main_window->window), action, "Cancel", GTK_RESPONSE_CANCEL,
        "Open", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT)
        file_selected(GTK_WIDGET(chooser), main_window);

    gtk_widget_destroy(dialog);
}

void cancel_image_selection(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_page(main_window, "page1");
}

void rotation_changed(GtkWidget *_, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *)user_data;

    gdouble value = gtk_range_get_value(
        GTK_RANGE(main_window->controls->rotation_scale));

    *main_window->images->image_rotated
        = rotate_image(main_window->images->image, value);

    main_window->images->is_rotated = TRUE;
    main_window->images->current_rotation = value;

    display_image(GTK_DRAWING_AREA(main_window->pages->page3->image),
        main_window->images->image_rotated, main_window);

    return;
}

void hide_grid(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    if (gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(main_window->controls->hide_grid_button)))
    {
        gtk_widget_hide(GTK_WIDGET(main_window->sudoku_labels_grid));
    }
    else
    {
        gtk_widget_show(GTK_WIDGET(main_window->sudoku_labels_grid));
    }
}

void hide_image(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    if (gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(main_window->controls->hide_image_button)))
    {
        gtk_widget_hide(GTK_WIDGET(main_window->pages->page4->image));
    }
    else
    {
        gtk_widget_show(GTK_WIDGET(main_window->pages->page4->image));
    }
}

gboolean grid_detection_finished(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    display_image(main_window->pages->page3->image,
        main_window->images->image_rotated_clean, main_window);

    set_step(main_window->step_indicators, 4);

    Image **cells = split_grid(main_window->images->image_rotated_cropped,
        VERBOSE_MODE, VERBOSE_PATH);

    for (int i = 0; i < 9; i++)
    {
        main_window->grid[i] = malloc(sizeof(int) * 9);
        for (int j = 0; j < 9; j++)
        {
            int c = i * 9 + j;
            int digit = neural_network_execute(cells[c]);
            free_Image(cells[c]);
            free(cells[c]);

            main_window->grid[i][j] = digit;
        }
    }
    free(cells);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            GtkLabel *label = GTK_LABEL(gtk_grid_get_child_at(
                GTK_GRID(main_window->sudoku_labels_grid), i, j));

            gchar label_text[100];
            g_snprintf(label_text, 100,
                "<span font='12' weight='bold'>%d</span>",
                main_window->grid[i][j]);

            gtk_label_set_markup(label, label_text);
        }
    }

    // Display elements in page 3
    display_image(main_window->pages->page3->image,
        main_window->images->image_rotated_cropped, main_window);

    // Display elements in page 4 and show it
    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated_cropped, main_window);
    set_page(main_window, "page4");

    // Reset page 3 state
    set_button_to_label(
        main_window->controls->image_rotation_done_button, "Done");
    gtk_widget_show(GTK_WIDGET(main_window->controls->rotation_scale));

    return FALSE;
}

void *grid_detection_handler(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    double rotation = 0;
    main_window->images->grid_square
        = grid_processing_detect_grid(main_window->images->image_rotated,
            &rotation, VERBOSE_MODE, VERBOSE_PATH);

    main_window->images->current_rotation += rotation;
    *main_window->images->image_rotated_clean = rotate_image(
        main_window->images->clean, main_window->images->current_rotation);

    main_window->images->image_rotated_cropped
        = correct_perspective(main_window->images->image_rotated_clean,
            main_window->images->grid_square, VERBOSE_MODE, VERBOSE_PATH);

    g_idle_add(grid_detection_finished, main_window);

    return NULL;
}

void process_image(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_button_to_load(main_window->controls->image_rotation_done_button);

    // If the image is not rotated, populate the image_rotated pointer
    if (!main_window->images->is_rotated)
        *main_window->images->image_rotated
            = clone_image(main_window->images->image);

    // Display elements in page 3
    gchar label[100];
    g_snprintf(label, 100,
        "<span weight=\"bold\" size=\"large\">Processing Image</span>");
    gtk_label_set_markup(main_window->pages->page3->label, label);
    gtk_widget_hide(GTK_WIDGET(main_window->controls->rotation_scale));
    set_step(main_window->step_indicators, 3);
    display_image(main_window->pages->page3->image,
        main_window->images->image_rotated, main_window);

    g_thread_new("grid_detection_handler", (GThreadFunc)grid_detection_handler,
        main_window);
}

void window_destroy(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    gtk_main_quit();
}

int main()
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
    GtkButton *correction_done_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "correctiondonebutton"));

    GtkToggleButton *hide_grid_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "hidegridbutton"));
    GtkToggleButton *hide_image_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "hideimagebutton"));

    GtkFileChooserButton *file_chooser_button = GTK_FILE_CHOOSER_BUTTON(
        gtk_builder_get_object(builder, "fileselector"));

    GtkScale *rotation_scale
        = GTK_SCALE(gtk_builder_get_object(builder, "rotationscale"));

    GtkGrid *sudoku_labels_grid
        = GTK_GRID(gtk_builder_get_object(builder, "sudokulabelsgrid"));

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

    GtkDrawingArea *page2_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page2image"));
    GtkDrawingArea *page3_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page3image"));
    GtkDrawingArea *page4_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page4image"));

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
        .correction_done_button = correction_done_button,
        .hide_grid_button = hide_grid_button,
        .hide_image_button = hide_image_button,
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
        .is_rotated = FALSE,
        .is_loaded = FALSE,
        .current_rotation = 0,
        .mask = malloc(sizeof(Image)),
        .image = malloc(sizeof(Image)),
        .clean = malloc(sizeof(Image)),
        .image_rotated = malloc(sizeof(Image)),
        .image_rotated_clean = malloc(sizeof(Image)),
        .image_rotated_cropped = malloc(sizeof(Image)),
        .display_size = 0,
        .current_container = malloc(sizeof(GtkDrawingArea)),
        .current_image = malloc(sizeof(Image)),
    };

    MainWindow main_window = {
        .window = GTK_WIDGET(window),
        .stack = stack,
        .sudoku_labels_grid = sudoku_labels_grid,
        .step_indicators = &step_indicators,
        .controls = &controls,
        .pages = &pages,
        .images = &images,
    };

    // Connect signals
    g_signal_connect(
        window, "destroy", G_CALLBACK(window_destroy), &main_window);

    // File selection
    g_signal_connect(file_chooser_button, "selection-changed",
        G_CALLBACK(file_selected), &main_window);
    // Save dialog
    g_signal_connect(
        save_button, "clicked", G_CALLBACK(save_current_image), &main_window);
    // Open dialog
    g_signal_connect(
        open_button, "clicked", G_CALLBACK(open_image), &main_window);
    // Previous page
    g_signal_connect(
        prev_button, "clicked", G_CALLBACK(previous_page), &main_window);

    g_signal_connect(
        hide_grid_button, "toggled", G_CALLBACK(hide_grid), &main_window);
    g_signal_connect(
        hide_image_button, "toggled", G_CALLBACK(hide_image), &main_window);

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

    // Images Drawing Area
    g_signal_connect(
        page2_image, "draw", G_CALLBACK(draw_image), &main_window);
    g_signal_connect(
        page3_image, "draw", G_CALLBACK(draw_image), &main_window);
    g_signal_connect(
        page4_image, "draw", G_CALLBACK(draw_image_and_grid), &main_window);

    main_window.grid = calloc(9, sizeof(int *));
    for (int i = 0; i < 9; i++)
        main_window.grid[i] = calloc(9, sizeof(int));

    set_step(&step_indicators, 1);

    gtk_widget_show(GTK_WIDGET(window));

    gtk_main();

    return 0;
}