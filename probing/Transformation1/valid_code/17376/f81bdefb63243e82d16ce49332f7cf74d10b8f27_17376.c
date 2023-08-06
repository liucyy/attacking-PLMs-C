void vga_hw_screen_dump(const char *filename)

{

    TextConsole *previous_active_console;



    previous_active_console = active_console;

    active_console = consoles[0];

    

    if (consoles[0] && consoles[0]->hw_screen_dump)

        consoles[0]->hw_screen_dump(consoles[0]->hw, filename);

    active_console = previous_active_console;

}