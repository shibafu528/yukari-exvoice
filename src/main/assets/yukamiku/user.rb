# encoding: utf-8

# User互換クラス
class User < Diva::Model

  register :twitter_user

  # args format
  # key     | value
  # --------+-----------
  # id      | user id
  # idname  | account name
  # nickname| account usual name
  # location| location(not required)
  # detail  | detail
  # profile_image_url | icon

  field.int    :id
  field.string :idname
  field.string :name
  field.string :location
  field.string :detail
  field.string :profile_image_url
  field.string :url
  field.bool   :protected
  field.bool   :verified
  field.int    :followers_count
  field.int    :statuses_count
  field.int    :friends_count

  def self.system
    if not defined? @@system then
      @@system = User.new({:id => 0,
                           :idname => 'mikutter_bot',
                           :name => Environment::NAME,
                           :profile_image_url => nil})
    end
    @@system
  end

  def initialize(value)
    super(value)
  end

  def idname
    self[:idname]
  end

  alias to_s idname

  def title
    "#{idname}(#{name})"
  end

  def protected?
    !!self[:protected]
  end

  def verified?
    !!self[:verified]
  end

  def profile_image_url_large
    url = self[:profile_image_url]
    if url
      url.gsub(/_normal(.[a-zA-Z0-9]+)\Z/, '\1')
    end
  end

  def inspect
    "User(@#{@value[:idname]})"
  end

  def ==(other)
    if other.is_a?(String) then
      @value[:idname] == other
    elsif other.is_a?(User) then
      other[:id] == self[:id]
    end
  end

  def me?(service)
    false
  end

  alias is_me? me?

  def user
    self
  end
  alias to_user user
end