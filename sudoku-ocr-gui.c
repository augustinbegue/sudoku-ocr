#include <gtk/gtk.h>
#include <image.h>

struct StepIndicators
{
    GtkWidget *step_1_indicator;
    GtkWidget *step_2_indicator;
    GtkWidget *step_3_indicator;
    GtkWidget *step_4_indicator;
    GtkWidget *step_5_indicator;
};
typedef struct StepIndicators StepIndicators;

struct Buttons
{
    GtkButton *save_button;
    GtkButton *open_button;
    GtkFileChooserButton *file_chooser_button;
    GtkButton *confirm_image_button;
    GtkButton *cancel_image_button;
};
typedef struct Buttons Buttons;

struct Page
{
    GtkViewport *container;
    GtkImage *image;
    GtkLabel *label;
};
typedef struct Page Page;

struct Pages
{
    Page *page1;
    Page *page2;
    Page *page3;
};
typedef struct Pages Pages;

struct MainWindow
{
    GtkWidget *window;
    GtkStack *stack;
    StepIndicators *step_indicators;
    Buttons *buttons;
    Pages *pages;
};
typedef struct MainWindow MainWindow;

void set_page(GtkStack *stack, const gchar *page)
{
    gtk_stack_set_visible_child_name(stack, page);
}

void set_step(StepIndicators *step_indicators, int step_number)
{
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

void file_selected(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GtkFileChooser *file_chooser
        = GTK_FILE_CHOOSER(main_window->buttons->file_chooser_button);

    GFile *file = gtk_file_chooser_get_file(file_chooser);
    char *path = g_file_get_path(file);

    Image image = SDL_Surface_to_Image(load_image(path));

    GdkPixbuf *pixbuf = image_to_pixbuf(&image);

    gtk_image_set_from_pixbuf(main_window->pages->page2->image, pixbuf);

    gchar label[100];
    g_snprintf(label, 100, "Image: %s", path);
    gtk_label_set_text(main_window->pages->page2->label, label);

    set_page(main_window->stack, "page2");
}

void image_canceled(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_page(main_window->stack, "page1");
}

void image_confirmed(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(main_window->pages->page2->image);
    gtk_image_set_from_pixbuf(main_window->pages->page3->image, pixbuf);

    gchar label[100];
    g_snprintf(label, 100,
        "<span weight=\"bold\" size=\"large\">Optimizing Image</span>");
    gtk_label_set_markup(main_window->pages->page3->label, label);

    set_step(main_window->step_indicators, 2);
    set_page(main_window->stack, "page3");
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
    GtkFileChooserButton *file_chooser_button = GTK_FILE_CHOOSER_BUTTON(
        gtk_builder_get_object(builder, "fileselector"));

    GtkButton *confirm_image_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "confirmimagebutton"));
    GtkButton *cancel_image_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "cancelimagebutton"));

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

    GtkImage *page2_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "page2image"));
    GtkImage *page3_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "page3image"));

    GtkLabel *page2_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page2label"));
    GtkLabel *page3_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page3label"));

    StepIndicators step_indicators = {
        .step_1_indicator = step_1_indicator,
        .step_2_indicator = step_2_indicator,
        .step_3_indicator = step_3_indicator,
        .step_4_indicator = step_4_indicator,
        .step_5_indicator = step_5_indicator,
    };

    Buttons buttons = {
        .save_button = save_button,
        .open_button = open_button,
        .file_chooser_button = file_chooser_button,
        .confirm_image_button = confirm_image_button,
        .cancel_image_button = cancel_image_button,
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

    Pages pages = {
        .page1 = &page1,
        .page2 = &page2,
        .page3 = &page3,
    };

    MainWindow main_window = {
        .window = window,
        .stack = stack,
        .step_indicators = &step_indicators,
        .buttons = &buttons,
        .pages = &pages,
    };

    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(file_chooser_button, "selection-changed",
        G_CALLBACK(file_selected), &main_window);
    g_signal_connect(confirm_image_button, "clicked",
        G_CALLBACK(image_confirmed), &main_window);
    g_signal_connect(cancel_image_button, "clicked",
        G_CALLBACK(image_canceled), &main_window);

    set_step(&step_indicators, 1);

    gtk_widget_show(GTK_WIDGET(window));

    gtk_main();

    return 0;
}