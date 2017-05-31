package info.shibafu528.yukari.exvoice.model;

import info.shibafu528.yukari.exvoice.diva.Model;

import javax.annotation.Nullable;

/**
 * Created by shibafu on 2017/05/30.
 */
public interface MessageMixin extends Model {
    boolean isMentionedByMe();

    /*Deferred*/ Object favorite(boolean fav);
    boolean isFavorite(@Nullable Object counterPart);
    Model[] getFavoritedBy();
    boolean isFavoritable(@Nullable Object counterPart);

    /*Deferred*/ Object retweet();
    boolean isRetweet();
    boolean isRetweeted(@Nullable Object counterPart);
    boolean getRetweetedBy();
    boolean isRetweetable(@Nullable Object counterPart);
    @Nullable Model getRetweetSource(boolean forceRetrieve);

    boolean isQuoting();
}
