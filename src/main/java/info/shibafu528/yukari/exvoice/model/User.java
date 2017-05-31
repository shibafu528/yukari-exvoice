package info.shibafu528.yukari.exvoice.model;

import info.shibafu528.yukari.exvoice.diva.Model;

/**
 * Created by shibafu on 2017/06/01.
 */
public interface User extends Model {
    long getId();
    String getIdname();
    String getName();
    String getLocation();
    String getDetail();
    String getProfileImageUrl();
    String getUrl();
    boolean isProtected();
    boolean isVerified();
    int getFollowersCount();
    int getStatusesCount();
    int getFriendsCount();
}
