# encoding: utf-8

# Message互換クラス
class Message < Diva::Model

  # args format
  # key     | value(class)
  #---------+--------------
  # id      | id of status(mixed)
  # entity  | entity(mixed)
  # message | posted text(String)
  # tags    | kind of message(Array)
  # user    | user who post this message(User or Hash or mixed(User IDNumber))
  # reciver | recive user(User)
  # replyto | source message(Message or mixed(Status ID))
  # retweet | retweet to this message(Message or StatusID)
  # post    | post object(Service)
  # image   | image(URL or Image object)

  field.int    :id, required: true
  field.string :message, required: true             # Message description
  field.has    :user, User, required: true          # Send by user
  field.int    :in_reply_to_user_id                 # リプライ先ユーザID
  field.has    :receiver, User                      # Send to user
  field.int    :in_reply_to_status_id               # リプライ先ツイートID
  field.has    :replyto, Message                    # Reply to this message
  field.has    :retweet, Message                    # ReTweet to this message
  field.string :source                              # using client
  field.bool   :exact                               # true if complete data
  field.time   :created                             # posted time
  field.time   :modified                            # updated time

  def initialize(value)
    super(value)
  end

  def idname
    user.idname
  end

  def system?
    false
  end

  def protected?
    if retweet?
      retweet_ancestor.protected?
    else
      user.protected?
    end
  end

  def verified?
    user.verified?
  end

  def user
    self[:user]
  end

  def has_receive_message?
    !!(self[:replyto] || self[:in_reply_to_status_id])
  end

  alias reply? has_receive_message?

  def retweet?
    !!self[:retweet]
  end

  def post(other, &proc)
    other[:replyto] = self
    other[:receiver] = self[:user]

    # TODO: mikutterではプライマリサービスが必ず優先されるけど、y4aでは代表ユーザ[PreformedStatus#getRepresentUser()]を使ったほうがいいんじゃないかな
    service = Service.primary
    if service.is_a? Service
      service.post(other)
    end
  end

  def message
    self
  end
end