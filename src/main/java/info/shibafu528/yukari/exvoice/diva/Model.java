package info.shibafu528.yukari.exvoice.diva;

import android.net.Uri;
import info.shibafu528.yukari.exvoice.MRubyPointer;

/**
 * Created by shibafu on 2017/05/30.
 */
public interface Model extends MRubyPointer {
    Uri getPermaLink();
    Uri getUri();
    boolean isMe();
}
