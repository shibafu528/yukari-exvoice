package info.shibafu528.yukari.exvoice.pluggaloid;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface Spell {
    /**
     * バインド対象のSpell名
     * @return
     */
    String value();

    /**
     * Modelのslugの配列
     * @return
     */
    String[] constraints();
}
