package info.shibafu528.yukari.exvoice.pluggaloid;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * {@link Spell} で残余キーワード引数をバインドするための指示アノテーション
 */
@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
public @interface RestKeywords {
}
