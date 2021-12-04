#include <gtk/gtk.h>
#include <pthread.h>
#include "grid_processing.h"
#include "grid_splitting.h"
#include "image.h"
#include "image_processing.h"
#include "perspective_correction.h"
#include "result_network.h"
#include "rotation.h"
#include "solver.h"

#define IMAGE_SIZE 500
#define VERBOSE_MODE 0
#define VERBOSE_PATH NULL
#define ACCENT_COLOR    \
    {                   \
        13, 137, 224, 1 \
    }
#define TRANSPARENT \
    {               \
        0, 0, 0, 0  \
    }

struct StepIndicators
{
    int current_step;
    GtkWidget *step_1_indicator;
    GtkWidget *step_2_indicator;
    GtkWidget *step_3_indicator;
    GtkWidget *step_4_indicator;
    GtkWidget *step_5_indicator;
    GtkWidget *step_6_indicator;
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
    GtkButton *confirm_crop_button;
    GtkButton *correction_done_button;
    GtkToggleButton *hide_grid_button;
    GtkToggleButton *hide_image_button;
    GtkFileChooserButton *file_chooser_button;
    GtkEventBox *crop_event_box;
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
    Page *page5;
    Page *page6;
};
typedef struct Pages Pages;

struct Images
{
    Image *current_image;
    Image *clean;
    Image *mask;
    Image *image;
    Image *image_rotated;
    Image *image_rotated_clean;
    Image *image_rotated_cropped;
    Image *image_solved;
    gdouble current_rotation;
    gboolean is_rotated;
    gboolean is_loaded;
    square *grid_square;
    gint display_size;
    GtkDrawingArea *current_container;
    gboolean editing_digits;
    GtkLabel *current_label;
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
    gboolean cropping;
    int **grid;
};
typedef struct MainWindow MainWindow;

/*
 * Images Display
 */

void draw_image_and_square(
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

    square *grid_square = main_window->images->grid_square;

    cairo_set_source_surface(cr, surface, scaled_x, scaled_y);
    cairo_rectangle(cr, scaled_x, scaled_y, scaled_width, scaled_height);
    cairo_fill(cr);

    main_window->images->display_size
        = scaled_width > scaled_height ? scaled_height : scaled_width;

    // Black filter on cropped part
    cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
    cairo_rectangle(cr, scaled_x, scaled_y, scaled_width, scaled_height);
    cairo_move_to(cr, scaled_x + grid_square->c1.x * scale,
        scaled_y + grid_square->c1.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c2.x * scale,
        scaled_y + grid_square->c2.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c3.x * scale,
        scaled_y + grid_square->c3.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c4.x * scale,
        scaled_y + grid_square->c4.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c1.x * scale,
        scaled_y + grid_square->c1.y * scale);
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_fill(cr);

    // Black Square
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 4);
    cairo_move_to(cr, scaled_x + grid_square->c1.x * scale,
        scaled_y + grid_square->c1.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c2.x * scale,
        scaled_y + grid_square->c2.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c3.x * scale,
        scaled_y + grid_square->c3.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c4.x * scale,
        scaled_y + grid_square->c4.y * scale);
    cairo_line_to(cr, scaled_x + grid_square->c1.x * scale,
        scaled_y + grid_square->c1.y * scale);
    cairo_stroke(cr);

    // Handle1
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_arc(cr, scaled_x + grid_square->c1.x * scale,
        scaled_y + grid_square->c1.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc(cr, scaled_x + grid_square->c1.x * scale,
        scaled_y + grid_square->c1.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);

    // Handle2
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_arc(cr, scaled_x + grid_square->c2.x * scale,
        scaled_y + grid_square->c2.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc(cr, scaled_x + grid_square->c2.x * scale,
        scaled_y + grid_square->c2.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);

    // Handle3
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_arc(cr, scaled_x + grid_square->c3.x * scale,
        scaled_y + grid_square->c3.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc(cr, scaled_x + grid_square->c3.x * scale,
        scaled_y + grid_square->c3.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);

    // Handle4
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_arc(cr, scaled_x + grid_square->c4.x * scale,
        scaled_y + grid_square->c4.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc(cr, scaled_x + grid_square->c4.x * scale,
        scaled_y + grid_square->c4.y * scale,
        main_window->images->display_size / 50, 0, 2 * G_PI);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);

    cairo_surface_destroy(surface);
    g_object_unref(pixbuf);
}

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
            GtkLabel *label = gtk_bin_get_child(GTK_EVENT_BOX(
                gtk_grid_get_child_at(main_window->sudoku_labels_grid, i, j)));

            gtk_widget_set_size_request(GTK_WIDGET(label),
                main_window->images->display_size / 9,
                main_window->images->display_size / 9);

            gchar label_text[100];
            g_snprintf(label_text, 100, "%ix%i", i, j);
            gtk_widget_set_name(GTK_WIDGET(label), label_text);

            if (main_window->grid[i][j] != 0)
            {
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

    cairo_surface_destroy(surface);
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

    cairo_surface_destroy(surface);
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

/*
 *  Pages and steps
 */

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
    gtk_widget_set_opacity(step_indicators->step_6_indicator, 0.5);

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
        case 6:
            gtk_widget_set_opacity(step_indicators->step_6_indicator, 1.0);
            break;
    }
}

/*
 *  Button states
 */

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

/*
 *  File Manipulations
 */

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

void previous_page(GtkWidget *widget, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    // TODO: Fix or Remove

    if (g_str_equal(main_window->pages->current_page, "page2"))
    {
        set_page(main_window, "page1");
        set_step(main_window->step_indicators, 1);
    }
    else if (g_str_equal(main_window->pages->current_page, "page4"))
    {
        set_step(main_window->step_indicators, 1);
        file_selected(NULL, main_window);
        set_page(main_window, "page2");
    }
    else if (g_str_equal(main_window->pages->current_page, "page5"))
    {
        file_selected(NULL, main_window);
        manual_rotate_image(NULL, main_window);
        set_page(main_window, "page4");
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

/*
 *  Page 6
 */

void solve_sudoku(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_step(main_window->step_indicators, 6);

    printf("Solving Sudoku\n");

    int grid[9][9];
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            grid[i][j] = main_window->grid[i][j];
        }
    }

    int original_grid[9][9];
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            original_grid[i][j] = grid[i][j];
        }
    }

    int **res = solvedGrid(grid);

    if (res != NULL)
    {
        gtk_widget_queue_draw(GTK_WIDGET(main_window->pages->page5->image));
    }
    else
    {
        printf("Sudoku Not Solved\n");
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "This grid could not be solved. Please correct the recognised "
            "digits and try again.",
            GTK_WINDOW(main_window->window), GTK_DIALOG_MODAL, "OK",
            GTK_RESPONSE_ACCEPT, NULL);

        gtk_dialog_run(GTK_DIALOG(dialog));
    }

    Image empty
        = SDL_Surface_to_Image(load_image("./assets/grids/blank-grid.png"));

    displayEmptyGrid(original_grid, grid, &empty);

    display_image(main_window->pages->page6->image, &empty, main_window);

    set_page(main_window, "page6");
}

/*
 *  Page 5
 */

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
        gtk_widget_hide(GTK_WIDGET(main_window->pages->page5->image));
    }
    else
    {
        gtk_widget_show(GTK_WIDGET(main_window->pages->page5->image));
    }
}

gboolean *grid_splitting_finished(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    // Display elements in page 4
    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated_cropped, main_window);

    // Display elements in page 5 and show it
    display_image(main_window->pages->page5->image,
        main_window->images->image_rotated_cropped, main_window);
    set_page(main_window, "page5");

    set_step(main_window->step_indicators, 5);

    // Reset page 5 state
    set_button_to_label(main_window->controls->confirm_crop_button, "Confirm");
    gtk_widget_show(GTK_WIDGET(main_window->controls->rotation_scale));

    return FALSE;
}

void *grid_splitting_handler(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

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

    g_idle_add(grid_splitting_finished, data);

    return NULL;
}

gboolean *perspective_correction_finished(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    display_image(GTK_DRAWING_AREA(main_window->pages->page4->image),
        main_window->images->image_rotated_cropped, main_window);

    g_thread_new("grid_splitting_handler", (GThreadFunc)grid_splitting_handler,
        main_window);

    return FALSE;
}

void *perspective_correction_handler(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    main_window->images->image_rotated_cropped
        = correct_perspective(main_window->images->image_rotated_clean,
            main_window->images->grid_square, VERBOSE_MODE, VERBOSE_PATH);

    g_idle_add(perspective_correction_finished, data);
}

void perspective_correction(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_button_to_load(main_window->controls->confirm_crop_button);

    g_thread_new("perspective_correction_handler",
        (GThreadFunc)perspective_correction_handler, main_window);
}

gboolean keypress_handler(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    if (event->keyval == GDK_KEY_Escape || event->keyval == GDK_KEY_Return)
    {
        if (main_window->images->editing_digits)
        {
            GdkRGBA col = TRANSPARENT;
            gtk_widget_override_background_color(
                main_window->images->current_label, GTK_STATE_FLAG_NORMAL,
                &col);
        }

        main_window->images->editing_digits = FALSE;
    }
    else if (event->keyval >= GDK_KEY_0 && GDK_KEY_9 >= event->keyval)
    {
        if (main_window->images->editing_digits)
        {
            gchar *name
                = gtk_widget_get_name(main_window->images->current_label);

            int i = name[0] - '0';
            int j = name[2] - '0';

            main_window->grid[i][j] = event->keyval - GDK_KEY_0;

            gtk_widget_queue_draw(
                GTK_WIDGET(main_window->pages->page5->image));
        }
    }
}

gboolean update_recognised_digits(
    GtkWidget *event_box, GdkEvent *event, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *)user_data;

    if (main_window->images->editing_digits)
    {
        GdkRGBA col = TRANSPARENT;
        gtk_widget_override_background_color(
            main_window->images->current_label, GTK_STATE_FLAG_NORMAL, &col);
    }

    GtkLabel *label = gtk_bin_get_child(GTK_EVENT_BOX(event_box));

    GdkRGBA col = ACCENT_COLOR;
    gtk_widget_override_background_color(label, GTK_STATE_FLAG_NORMAL, &col);

    main_window->images->editing_digits = TRUE;
    main_window->images->current_label = label;
}

/*
 *  Page 4
 */

void update_grid_square_pos(
    GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *)user_data;

    GdkEventMotion *e = (GdkEventMotion *)event;
    int x = (int)e->x;
    int y = (int)e->y;

    // Find the closest point of the grid square
    double closest_distance = 1000000;
    square *grid_square = main_window->images->grid_square;
    point *closest_point = &grid_square->c1;

    gint width = gtk_widget_get_allocated_width(
        GTK_WIDGET(main_window->pages->page4->image));
    gint height = gtk_widget_get_allocated_height(
        GTK_WIDGET(main_window->pages->page4->image));

    gint image_width = main_window->images->current_image->width;
    gint image_height = main_window->images->current_image->height;

    gdouble scale_x = (gdouble)width / image_width;
    gdouble scale_y = (gdouble)height / image_height;
    gdouble scale = MIN(scale_x, scale_y);

    gint scaled_width = image_width * scale;
    gint scaled_height = image_height * scale;
    gint scaled_x = (width - scaled_width) / 2;
    gint scaled_y = (height - scaled_height) / 2;

    x = (x - scaled_x) / scale;
    y = (y - scaled_y) / scale;

    point *points[4] = {&grid_square->c1, &grid_square->c2, &grid_square->c3,
        &grid_square->c4};

    for (int i = 0; i < 4; i++)
    {
        int x_diff = x - points[i]->x;
        int y_diff = y - points[i]->y;
        double distance = sqrt(x_diff * x_diff + y_diff * y_diff);

        if (distance < closest_distance)
        {
            closest_distance = distance;
            closest_point = points[i];
        }
    }

    closest_point->x = x;
    closest_point->y = y;

    // Update the grid square
    gtk_widget_queue_draw(GTK_DRAWING_AREA(main_window->pages->page4->image));

    return;
}

gboolean grid_detection_finished(gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated_clean, main_window);

    set_step(main_window->step_indicators, 4);

    set_page(main_window, "page4");

    // TODO: Correct square

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

    g_idle_add(grid_detection_finished, main_window);

    return NULL;
}

void process_image(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_button_to_load(main_window->controls->image_rotation_done_button);
    gtk_widget_hide(GTK_WIDGET(main_window->controls->rotation_scale));

    // If the image is not rotated, populate the image_rotated pointer
    if (!main_window->images->is_rotated)
        *main_window->images->image_rotated
            = clone_image(main_window->images->image);

    // Display elements in page 4
    gchar label[100];
    g_snprintf(label, 100,
        "<span weight=\"bold\" size=\"large\">Processing Image</span>");
    gtk_label_set_markup(main_window->pages->page4->label, label);
    gtk_widget_hide(GTK_WIDGET(main_window->controls->rotation_scale));
    set_step(main_window->step_indicators, 3);
    display_image(main_window->pages->page4->image,
        main_window->images->image_rotated, main_window);

    g_thread_new("grid_detection_handler", (GThreadFunc)grid_detection_handler,
        main_window);
}

/*
 *  Page 3
 */

void cancel_image_selection(GtkWidget *_, gpointer data)
{
    MainWindow *main_window = (MainWindow *)data;

    set_page(main_window, "page1");
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

void window_destroy(GtkWidget *_, gpointer data)
{
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
    GtkButton *confirm_crop_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "confirmcropbutton"));
    GtkButton *correction_done_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "correctiondonebutton"));
    GtkToggleButton *hide_grid_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "hidegridbutton"));
    GtkToggleButton *hide_image_button
        = GTK_BUTTON(gtk_builder_get_object(builder, "hideimagebutton"));
    GtkEventBox *crop_event_box
        = GTK_EVENT_BOX(gtk_builder_get_object(builder, "cropeventbox"));

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
    GtkWidget *step_6_indicator
        = GTK_WIDGET(gtk_builder_get_object(builder, "step6indicator"));

    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack1"));

    GtkViewport *page1_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page1"));
    GtkViewport *page2_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page2"));
    GtkViewport *page3_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page3"));
    GtkViewport *page4_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page4"));
    GtkViewport *page5_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page5"));
    GtkViewport *page6_container
        = GTK_VIEWPORT(gtk_builder_get_object(builder, "page6"));

    GtkDrawingArea *page2_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page2image"));
    GtkDrawingArea *page3_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page3image"));
    GtkDrawingArea *page4_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page4image"));
    GtkDrawingArea *page5_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page5image"));
    GtkDrawingArea *page6_image
        = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "page6image"));

    GtkLabel *page2_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page2label"));
    GtkLabel *page3_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page3label"));
    GtkLabel *page4_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page4label"));
    GtkLabel *page5_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page5label"));
    GtkLabel *page6_label
        = GTK_LABEL(gtk_builder_get_object(builder, "page6label"));

    StepIndicators step_indicators = {
        .current_step = 1,
        .step_1_indicator = step_1_indicator,
        .step_2_indicator = step_2_indicator,
        .step_3_indicator = step_3_indicator,
        .step_4_indicator = step_4_indicator,
        .step_5_indicator = step_5_indicator,
        .step_6_indicator = step_6_indicator,
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
        .confirm_crop_button = confirm_crop_button,
        .correction_done_button = correction_done_button,
        .hide_grid_button = hide_grid_button,
        .hide_image_button = hide_image_button,
        .crop_event_box = crop_event_box,
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
    Page page5 = {
        .container = page5_container,
        .image = page5_image,
        .label = page5_label,
    };
    Page page6 = {
        .container = page6_container,
        .image = page6_image,
        .label = page6_label,
    };

    Pages pages = {
        .current_page = "page1",
        .page1 = &page1,
        .page2 = &page2,
        .page3 = &page3,
        .page4 = &page4,
        .page5 = &page5,
        .page6 = &page6,
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
        .current_label = malloc(sizeof(GtkLabel)),
        .editing_digits = FALSE,
    };

    MainWindow main_window = {
        .window = GTK_WIDGET(window),
        .stack = stack,
        .sudoku_labels_grid = sudoku_labels_grid,
        .step_indicators = &step_indicators,
        .controls = &controls,
        .pages = &pages,
        .images = &images,
        .cropping = FALSE,
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

    // Confirm crop
    g_signal_connect(confirm_crop_button, "clicked",
        G_CALLBACK(perspective_correction), &main_window);

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
        page4_image, "draw", G_CALLBACK(draw_image_and_square), &main_window);
    g_signal_connect(
        page5_image, "draw", G_CALLBACK(draw_image_and_grid), &main_window);
    g_signal_connect(
        page6_image, "draw", G_CALLBACK(draw_image), &main_window);

    // Image cropping
    g_signal_connect(G_OBJECT(crop_event_box), "motion-notify-event",
        G_CALLBACK(update_grid_square_pos), &main_window);

    // Recognised Digits Editing
    g_signal_connect(G_OBJECT(window), "key-press-event",
        G_CALLBACK(keypress_handler), &main_window);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            GtkEventBox *evbox = GTK_EVENT_BOX(
                gtk_grid_get_child_at(sudoku_labels_grid, i, j));

            // TODO: find a way to transfer coordinates to edit grid directly
            g_signal_connect(evbox, "button-press-event",
                G_CALLBACK(update_recognised_digits), &main_window);
        }
    }

    // Grid Solving
    g_signal_connect(G_OBJECT(correction_done_button), "clicked",
        G_CALLBACK(solve_sudoku), &main_window);

    main_window.grid = calloc(9, sizeof(int *));
    for (int i = 0; i < 9; i++)
        main_window.grid[i] = calloc(9, sizeof(int));

    set_step(&step_indicators, 1);

    gtk_widget_show(GTK_WIDGET(window));

    gtk_main();

    return 0;
}