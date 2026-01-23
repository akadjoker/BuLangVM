#include "bindings.hpp"
#include <gtk/gtk.h>
#include <string>
#include <vector>

namespace GTKBindings
{
    // =============================================================
    // SISTEMA DE CALLBACKS (A parte difícil do GTK + VM)
    // =============================================================
    
    // Estrutura para saber qual função do script chamar
    struct CallbackPayload 
    {
        Interpreter* vm;
        std::string funcName;
    };

    // Callback genérico C++ que o GTK chama, e que repassa para a VM
    static void generic_signal_handler(GtkWidget *widget, gpointer data)
    {
        CallbackPayload* payload = (CallbackPayload*)data;
        // Chama a função no script BuLang

        Info("Chamando funcao %s", payload->funcName.c_str());

        payload->vm->callFunction(payload->funcName.c_str(),0);
    }

    // =============================================================
    // FUNÇÕES NATIVAS
    // =============================================================

    Value native_gtkInit(Interpreter *vm, int argc, Value *args) {
        // Inicializa o GTK (ignora args da command line por agora)
        gtk_init(0, NULL);
        return vm->makeNil();
    }

    Value native_gtkMain(Interpreter *vm, int argc, Value *args) {
        // Entra no loop infinito do GTK
        gtk_main();
        return vm->makeNil();
    }
    
    Value native_gtkMainQuit(Interpreter *vm, int argc, Value *args) {
        gtk_main_quit();
        return vm->makeNil();
    }

    Value native_gtkWindowNew(Interpreter *vm, int argc, Value *args) {
        // Cria janela
        GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        
        // Garante que o X fecha a app e não deixa o processo zombie
        g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        
        return vm->makePointer(win);
    }

    Value native_gtkSetTitle(Interpreter *vm, int argc, Value *args) {
        GtkWindow *win = GTK_WINDOW(args[0].asPointer());
        const char* title = args[1].asStringChars();
        gtk_window_set_title(win, title);
        return vm->makeNil();
    }

    Value native_gtkSetSize(Interpreter *vm, int argc, Value *args) {
        GtkWindow *win = GTK_WINDOW(args[0].asPointer());
        gtk_window_set_default_size(win, args[1].asNumber(), args[2].asNumber());
        return vm->makeNil();
    }

    Value native_gtkButtonNew(Interpreter *vm, int argc, Value *args) {
        const char* label = args[0].asStringChars();
        return vm->makePointer(gtk_button_new_with_label(label));
    }

    Value native_gtkContainerAdd(Interpreter *vm, int argc, Value *args) {
        GtkContainer *container = GTK_CONTAINER(args[0].asPointer());
        GtkWidget *widget = (GtkWidget*)args[1].asPointer();
        gtk_container_add(container, widget);
        return vm->makeNil();
    }

    Value native_gtkWidgetShowAll(Interpreter *vm, int argc, Value *args) {
        gtk_widget_show_all((GtkWidget*)args[0].asPointer());
        return vm->makeNil();
    }

    // =============================================================
    // LIGAR O SCRIPT AO C++ (g_signal_connect)
    // =============================================================

     CallbackPayload* payload = nullptr;
    
    Value native_gSignalConnect(Interpreter *vm, int argc, Value *args) {
        // Args: widget (pointer), "sinal" (string), "nomeFuncaoScript" (string)
        if (argc != 3) 
        {
            // Error("gSignalConnect expects 3 args");
            return vm->makeNil();
        }

        GtkWidget *widget = (GtkWidget*)args[0].asPointer();
        const char* signalName = args[1].asStringChars();
        const char* scriptFuncName = args[2].asStringChars();

        // Alocar payload (NOTA:  precisas gerir esta memória para não haver leaks)
        payload = new CallbackPayload();
        payload->vm = vm;
        payload->funcName = scriptFuncName;

        g_signal_connect(widget, signalName, G_CALLBACK(generic_signal_handler), payload);
        
        return vm->makeNil();
    }

    // =============================================================
    // REGISTO
    // =============================================================
    void gtkCleanAll( )
    {
            delete payload;
        
    }
    
    void registerAll(Interpreter &vm)
    {
            ModuleBuilder module = vm.addModule("gtk");
            module.addFunction("gtkInit", native_gtkInit, 0)
            .addFunction("gtkMain", native_gtkMain, 0)
            .addFunction("gtkMainQuit", native_gtkMainQuit, 0)
            .addFunction("gtkWindowNew", native_gtkWindowNew, 0)
            .addFunction("gtkSetTitle", native_gtkSetTitle, 2)
            .addFunction("gtkSetSize", native_gtkSetSize, 3)
            .addFunction("gtkButtonNew", native_gtkButtonNew, 1)
            .addFunction("gtkContainerAdd", native_gtkContainerAdd, 2)
            .addFunction("gtkWidgetShowAll", native_gtkWidgetShowAll, 1)
            .addFunction("gSignalConnect", native_gSignalConnect, 3);
    }
}