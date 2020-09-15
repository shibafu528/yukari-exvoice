# encoding: utf-8
Android.require_assets 'yukamiku/mikuenv.rb'
Android.require_assets 'yukamiku/gui.rb'
Android.require_assets 'yukamiku/gtk.rb'
Android.require_assets 'yukamiku/configloader.rb'

class Message
  field.uri :perma_link
end

module Plugin::YukaMiku
  class << self

    # twicca extrasをmikutter messageに変換する。
    # @param [Hash] extra twicca extras
    # @return [Message] mikutter message
    def to_message(extra)
      value = {
          perma_link: extra['url'],
          id: extra['id'].to_i,
          message: extra['text'],
          user: to_user(extra),
          receiver: nil,
          in_reply_to_user_id: extra['in_reply_to_user_id'].to_i,
          in_reply_to_status_id: extra['in_reply_to_status_id'].to_i,
          replyto: nil,
          retweet: nil,
          source: extra['source'],
          geo: "#{extra['latitude']}, #{extra['longitude']}",
          exact: false,
          created: nil, #extra['created_at'],
          modified: nil #extra['created_at']
      }
      Message.new(value)
    end

    # twicca extrasをmikutter userに変換する
    # @param [Hash] extra twicca extras
    # @return [User] mikutter user
    def to_user(extra)
      value = {
          id: extra['user_id'],
          idname: extra['user_screen_name'],
          name: extra['user_name'],
          profile_image_url: extra['user_profile_image_url_bigger']
      }
      User.new(value)
    end

  end
end

#
# ゆかミクは尊い (command DSLをtwicca DSLに変換します)
#
Plugin.create :yukamiku do

  filter_twicca_action_show_tweet do |actions|
    commands, = Plugin.filtering(:command, Hash.new)
    commands.each do |slug, command|
      next if command[:role] != :timeline
      puts "mikutter_command #{slug}(role: :timeline) => twicca_action :show_tweet"
      
      # パラメータの互換性対応
      command = command.melt
      command[:label] = command[:name]

      orig_exec = command[:exec]
      command[:exec] = Proc.new do |extra|
        opt = Plugin::GUI::Event.new(:contextmenu, Plugin[:gtk].widgetof(:timeline), [Plugin::YukaMiku.to_message(extra)])

        # Postboxの変化を監視するために現在値を保持
        postbox = Plugin[:gtk].widgetof(:postbox)
        postbox_before_text = postbox.widget_post.buffer.text
        postbox_before_options = postbox.options

        orig_exec.call(opt)

        # Postboxの内容が変化していたらツイート画面を出す
        if postbox.widget_post.buffer.text != postbox_before_text
          Plugin.call(:intent, activity: :TweetActivity, mode: :tweet, text: postbox.widget_post.buffer.text)
        elsif postbox.options != postbox_before_options
          call_opt = {activity: :TweetActivity, text: [postbox.options[:header], postbox.options[:footer]].join(' ').strip}

          if postbox.options.has_key? :to
            call_opt[:mode] = :reply
            call_opt[:in_reply_to] = postbox.options[:to][:id]
          else
            call_opt[:mode] = :tweet
          end

          Plugin.call(:intent, call_opt)
        end
      end

      actions[slug] = command
    end
    [actions]
  end

  filter_twicca_action_edit_tweet do |actions|
    commands, = Plugin.filtering(:command, Hash.new)
    commands.each do |slug, command|
      next if command[:role] != :postbox
      puts "mikutter_command #{slug}(role: :postbox) => twicca_action :edit_tweet"
      
      # パラメータの互換性対応
      command = command.melt
      command[:label] = command[:name]

      orig_exec = command[:exec]
      command[:exec] = Proc.new do |extra|
        opt = Plugin::GUI::Event.new(:contextmenu, Plugin[:gtk].widgetof(:postbox), [])

        # Postboxに現在の入力内容を投入する
        postbox = Plugin[:gtk].widgetof(:postbox)
        postbox.widget_post.buffer.text = extra['text']

        orig_exec.call(opt)

        # Postboxの内容を結果として返却する
        {result_code: :ok, intent: {text: postbox.widget_post.buffer.text}}
      end

      actions[slug] = command
    end
    [actions]
  end
end
