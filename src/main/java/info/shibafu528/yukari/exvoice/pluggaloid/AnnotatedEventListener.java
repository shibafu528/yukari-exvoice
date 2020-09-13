package info.shibafu528.yukari.exvoice.pluggaloid;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;

/*package*/ class AnnotatedEventListener implements Plugin.PluggaloidEventListener {
    private final Plugin plugin;
    private final Method method;

    /*package*/ AnnotatedEventListener(Plugin plugin, Method method) {
        this.plugin = plugin;
        this.method = method;
    }

    @Override
    public void onEvent(Object... args) {
        try {
            method.invoke(plugin, Arrays.copyOf(args, method.getParameterTypes().length));
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e);
        } catch (InvocationTargetException e) {
            Throwable cause = e.getCause();
            if (cause instanceof Error) {
                throw (Error) cause;
            } else if (cause instanceof RuntimeException) {
                throw (RuntimeException) cause;
            } else {
                throw new RuntimeException(cause);
            }
        }
    }
}
