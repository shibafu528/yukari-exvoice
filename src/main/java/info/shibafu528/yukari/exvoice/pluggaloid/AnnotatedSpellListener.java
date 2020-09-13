package info.shibafu528.yukari.exvoice.pluggaloid;

import android.text.TextUtils;

import java.lang.annotation.Annotation;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Map;

/*package*/ class AnnotatedSpellListener implements Plugin.PluggaloidSpellListener {
    private final Plugin plugin;
    private final Method method;
    private final int kwrestIndex;
    private final Map<String, Integer> keywordMap;

    /*package*/ AnnotatedSpellListener(Plugin plugin, Method method, Spell annotation) {
        this.plugin = plugin;
        this.method = method;

        // 引数のバインド位置を決める
        // NOTE: キーワード引数は必ずモデル引数の後に置く必要がある。さもないと壊れる。
        final Annotation[][] parameterAnnotations = method.getParameterAnnotations();
        int modelBinds = 0; // モデル引数をバインドする数
        int kwrestIndex = -1; // 残余キーワードをバインドする引数index (-1 = バインドしない)
        Map<String, Integer> keywordMap = new HashMap<>(); // キーワード名 -> 引数index
        for (int paramIndex = 0; paramIndex < parameterAnnotations.length; paramIndex++) {
            Annotation[] annotations = parameterAnnotations[paramIndex];
            boolean isKeyword = false;
            for (Annotation a : annotations) {
                if (a instanceof Keyword) {
                    String keyword = ((Keyword) a).value();
                    if (TextUtils.isEmpty(keyword)) {
                        throw new IllegalArgumentException(String.format(Locale.US, "%s() のKeywordアノテーションにキーワード名が設定されていません。", method.getName()));
                    }
                    keywordMap.put(keyword, paramIndex);
                    isKeyword = true;
                } else if (a instanceof RestKeywords) {
                    if (kwrestIndex != -1) {
                        throw new IllegalArgumentException(String.format(Locale.US, "%s() の引数にRestKeywordsアノテーションが複数指定されています。最大1つしか指定することはできません。", method.getName()));
                    }
                    kwrestIndex = paramIndex;
                    isKeyword = true;
                }
            }
            if (!isKeyword) {
                modelBinds++;
            }
        }

        if (annotation.constraints().length != modelBinds) {
            throw new IllegalArgumentException(String.format(Locale.US, "%s() のSpellアノテーション上のconstraintsと引数の数が一致していません。", method.getName()));
        }

        this.kwrestIndex = kwrestIndex;
        this.keywordMap = keywordMap;
    }

    @Override
    public void onSpell(Object[] models, Map<String, Object> options) {
        Object[] args = new Object[method.getParameterTypes().length];
        System.arraycopy(models, 0, args, 0, models.length);

        // キーワード引数のバインド
        Map<String, Object> kwrest = options;
        if (!keywordMap.isEmpty()) {
            kwrest = new LinkedHashMap<>();
            for (Map.Entry<String, ?> entry : options.entrySet()) {
                Integer position = keywordMap.get(entry.getKey());
                if (position != null) {
                    args[position] = entry.getValue();
                } else {
                    // 残余送り
                    kwrest.put(entry.getKey(), entry.getValue());
                }
            }
        }

        // 残余キーワードのバインド
        if (kwrestIndex != -1) {
            args[kwrestIndex] = kwrest;
        }

        try {
            method.invoke(plugin, args);
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
