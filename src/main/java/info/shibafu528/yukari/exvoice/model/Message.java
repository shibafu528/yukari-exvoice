package info.shibafu528.yukari.exvoice.model;

import java.util.Date;

/**
 * Created by shibafu on 2017/06/01.
 */
public interface Message extends MessageMixin {
    long getId();
    String getMessage();
    User getUser();
    long getInReplyToUserId();
    User getReceiver();
    long getInReplyToStatusId();
    Message getReplyto();
    Message getRetweet();
    String getSource();
    Date getCreated();
    Date getModified();
}
