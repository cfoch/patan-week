from gi.repository import Gtk, Vte
from gi.repository import GLib
import os

class PatanTerminal(Vte.Terminal):
    def __init__(self, especialidades, alumnos, fiestas, asistencias, *args, **kwds):
        super(PatanTerminal, self).__init__(*args, **kwds)
        self.fork_command_full(
            Vte.PtyFlags.DEFAULT,
            os.getcwd(),
            ["/bin/sh"],
            [],
            GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            None,
            None,
            )
        self.send_input("./main")
        self.send_input(especialidades)
        self.send_input(alumnos)
        self.send_input(fiestas)
        self.send_input(asistencias)

    def send_input(self, command):
        command = command + "\n"
        self.feed_child(command, len(command))

class PatanWeek:
    def __init__(self):
        builder = Gtk.Builder()
        builder.add_from_file("ui/ui.glade")

        self.window = Gtk.Window()
        self.window.connect("delete-event", Gtk.main_quit)

        self.especialidades_button = builder.get_object("especialidades")
        self.alumnos_button = builder.get_object("alumnos")
        self.fiestas_button = builder.get_object("fiestas")
        self.asistencias_button = builder.get_object("asistencias")

        self.container = builder.get_object("cont")
        self.run_button = Gtk.Button("Run!")
        self.container.pack_start(self.run_button, True, True, 0)
        self.run_button.connect("clicked", self.run_button_cb)

        self.window.add(self.container)
        self.window.show_all()

    def run_button_cb(self, w):
        especialidades =  self.especialidades_button.get_filename()
        alumnos = self.alumnos_button.get_filename()
        fiestas = self.fiestas_button.get_filename()
        asistencias = self.asistencias_button.get_filename()

        self.container.remove(self.run_button)

        vte = PatanTerminal(especialidades, alumnos, fiestas, asistencias)
        vte.show()
        self.container.pack_start(vte, True, True, 0)
    
"""
win = Gtk.Window()
win.connect('delete-event', Gtk.main_quit)
bigbox = Gtk.Box()
win.add(bigbox)
bigbox.add(PatanTerminal())
win.show_all()
"""
PatanWeek()
Gtk.main()
