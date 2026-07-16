<template>
	<view class="content">
		<view class="section">			
			<view class="left">账号ID: </view>
			<view class="right">
				<input class="input" v-model="account"></input>
			</view>
		</view>
		<view class="section">
			<view class="left">appkey: </view>
			<view class="right">
				<input class="input" v-model="appkey"></input>
			</view>
		</view>
		<view class="section">
			<view class="left">使用p8证书: </view>
			<view class="right">
				<switch :checked="usingP8Certificate" @change="changeApnsCertificate" />
			</view>
		</view>
		<view class="section">
			<view class="left">当前状态: </view>
			<view class="right">{{connState}}</view>
		</view>
		<view class="section">
			<view class="center">
				<button size="mini" class="btn connect" type="default" @click="connect">连接</button>		
				<button size="mini" class="btn destroy" type="default" @click="destroy">销毁</button>
				<button size="mini" class="btn destroy" type="default" @click="clearLog">清空日志</button>
				<button size="mini" class="btn destroy" type="default" @click="applyNotificationPermission">IOS申请通知权限</button>
			</view>
		</view>
		
		<view class="section">
			<view class="left">要聊天的会话ID: </view>
			<view class="right">
				<input class="input" v-model="conversationId"></input>
			</view>
		</view>
		<view class="section">
			<view class="center">
				<button size="mini" class="btn connect" type="default" @click="sendMessage">发一条消息</button>
			</view>
		</view>
		<scroll-view :scroll-y="true" :scroll-top="scrollTop" class="log-wrap">
			<rich-text class="log" :nodes="logArr"></rich-text>
		</scroll-view>
	</view>
</template>

<script>
const { log, debug, info, warn, error } = console
// const NIMSDK = require('../../static/NIM_UNIAPP_SDK')
import NIMSDK from 'nim-web-sdk-ng/dist/v2/NIM_UNIAPP_SDK'

function getGlobalData() {
  const app = getApp()
  app.globalData = app.globalData || {}
  return app.globalData
}

// 以下信息均为敏感信息，请开发者替换为自己的账号、appkey、token 及各厂商推送配置
const YOUR_ACCOUNT = '*********'
const CONVERSATION_ID = '*********'
const APPKEY = '*********'
const STATIC_TOKEN = '*********'
const OFFLINE_PUSH_CONFIG = {
  miPush: {
    appId: '*********',
    appKey: '*********',
    certificateName: '*********'
  },
  vivoPush: {
    certificateName: '*********'
  },
  oppoPush: {
    appId: '*********',
    appKey: '*********',
    secret: '*********',
    certificateName: '*********'
  },
  hwPush: {
    appId: '*********',
    certificateName: '*********'
  },
  fcmPush: {
    certificateName: '*********'
  },
  mzPush: {
    appId: '*********',
    appKey: '*********',
    certificateName: '*********'
  },
  honorPush: {
    certificateName: '*********'
  },
  apns: {
    certificateName: '*********'
  },
	// 新加的 ios pushkit 证书传递
	pushkit: {
		certificateName: '*********'
	},

  // 鸿蒙的配置
  harmonyPush: {
    certificateName: '*********'
  }
}

export default {
  data() {
    return {
      account: YOUR_ACCOUNT,
      appkey: APPKEY,
			conversationId: CONVERSATION_ID,
      connState: 'disconnect',
      logArr: [],
      scrollTop: 0,
      scrollTimeout: 0,
      scrollFlag: true,
      usingP8Certificate: true,
      // 推送唤起传入参数
      sessionId: '',
      sessionType: -1,
    }
  },
  onLoad(options) {
		// 根据缓存设置前一次登录的值
		const accountId = uni.getStorageSync('accountId')
		const token = uni.getStorageSync('token')
		if (accountId) this.account = accountId
		if (token) this.token = token
		
		// 这里处理推送拉起跳转过来的参数
    console.log('sessionId===', options.sessionId);
    console.log('sessionType===', options.sessionType);
    this.sessionId = options.sessionId
    this.sessionType = +options.sessionType
		// 简单使用: 根据推送入参来篡改 会话 id
		if (this.sessionId) {
			// 兼容原生 APP 的老版本 IM, 所以映射关系为 
			// 老版本的 0: 单聊，1：群聊，5：超级群
			// 新版本 1 单聊, 2 群聊, 3 超级群聊
			const conversationType = 			
			  this.sessionType === 0 ? 1 : this.sessionType === 1 ? 2 : 3
			this.conversationId = `${this.account}|${conversationType}|${this.sessionId}`
		}
  },
  mounted() {
    console.log("!!!!!! mounted!!!!!!")
    const that = this;
    console.log = function () {
      log.apply(console, Array.prototype.slice.call(arguments))
      that.refreshLog('grey', arguments[0])
      // that.scrollToBottom()
    }.bind(console)
    console.error = function () {
      error.apply(console, Array.prototype.slice.call(arguments))
      that.refreshLog('red',arguments[0])
      // that.scrollToBottom()
    }.bind(console)
  },
  onUnload() {
    clearInterval(this.scrollTimeout);
    console.log = log.bind(console);
    console.error = error.bind(console);
  },
  methods: {
    async connect () {
      const globalData = getGlobalData()
      globalData.nim = NIMSDK.getInstance({
        appkey: this.appkey,
				apiVersion: 'v2',
        debugLevel: 'debug',
        logger: console
      },
				{
					// "V2NIMLoginServiceConfig": {
					// 	"lbsUrls": [
					// 		"https://imtest-gy.netease.im/lbs/webconf"
					// 	],
					// 	"linkUrl": "weblink-dev-gy.netease.im:443"
					// },
				},
			);
  
      globalData.nim.V2NIMLoginService.on('onLoginStatus', function(arg1) {
        console.log('收到 V2NIMLoginService 模块的 onLoginStatus 事件', arg1)
      })
      
      // #ifdef APP-PLUS
      const nimPushPlugin = uni.requireNativePlugin('NIMUniPlugin-PluginModule')
      globalData.nim.V2NIMSettingService.setOfflinePushConfig(nimPushPlugin, OFFLINE_PUSH_CONFIG)
      // #endif

      // #ifdef APP-HARMONY
			const sysInfo = uni.getSystemInfoSync();
			globalData.nim.V2NIMSettingService.setOfflinePushConfig(uni.NIMNativePush, OFFLINE_PUSH_CONFIG)
			// #endif
      
      this.connState = 'connecting'
      await globalData.nim.V2NIMLoginService.login(this.account, STATIC_TOKEN, {
        "retryCount": 3,
        "timeout": 60000,
        "forceMode": false,
        "authType": 0,
      })
			uni.setStorageSync('accountId', this.account)
			uni.setStorageSync('token', STATIC_TOKEN)
    },
    destroy() {
      const globalData = getGlobalData()
      if (globalData.nim) {
        globalData.nim.destroy()
        globalData.nim = null
      }
    },
		async sendMessage() {
      const globalData = getGlobalData()
			if (!globalData.nim) {
				console.log('尚未初始化')
				return
			}
			const message = globalData.nim.V2NIMMessageCreator.createTextMessage('hello world')
			const senderId = globalData.nim.V2NIMLoginService.getLoginUser() // 发送着, 本账号
			const receiverId = globalData.nim.V2NIMConversationIdUtil.parseConversationTargetId(this.conversationId) // 接收者
			const conversationType = globalData.nim.V2NIMConversationIdUtil.parseConversationType(this.conversationId) // 1: 单聊，2：群聊，3：超级群
			/**  组织推送跳转内容 pushPayload 需要的参数 */
			// 单聊 p2p 会话, sessionId 要填本账号 id. 群聊要填群 id.
			const sessionId =
			  conversationType === 1
			    ? senderId
			    : receiverId
			// 兼容原生 APP 的老版本 IM, 所以映射关系为 老版本的 0: 单聊，1：群聊，5：超级群
			const sessionType =
			  conversationType === '1' ? '0' : conversationType === 2 ? '1' : '5'
			// 需要换成自己的包名
			const PACKAGE_NAME = 'com.netease.nim.demo'
			const pushPayload = JSON.stringify({
			  // xiaomi
			  notify_effect: '2', //可选项，预定义通知栏消息的点击行为。1：通知栏点击后打开app的Launcher Activity，2：通知栏点击后打开app的任一Activity（开发者还需要传入intent_uri），3：通知栏点击后打开网页（开发者还需要传入web_uri）
			  intent_uri: `intent:#Intent;action=com.netease.nimlib.uniapp.push.NotificationClickActivity;component=${PACKAGE_NAME}/com.netease.nimlib.uniapp.push.NotificationClickActivity;launchFlags=0x04000000;i.sessionType=${sessionType};S.sessionId=${sessionId};end`, //可选项，打开当前app的任一组件。
			  // huawei
			  hwField: {
			    click_action: {
			      //必填，消息点击行为
			      type: 1, //必填，消息点击行为类型，取值如下：1：打开应用自定义页面 2：点击后打开特定URL 3：点击后打开应用
			      // 自定义页面中intent的实现，请参见指定intent参数​。当type为1时，字段intent和action至少二选一。scheme方式和指定activity方式都可以
			      intent: `intent:#Intent;action=com.netease.nimlib.uniapp.push.NotificationClickActivity;component=${PACKAGE_NAME}/com.netease.nimlib.uniapp.push.NotificationClickActivity;launchFlags=0x04000000;i.sessionType=${sessionType};S.sessionId=${sessionId};end`,
			    },
			    androidConfig: {
			      category: 'IM', //可选项，标识消息类型，用于标识高优先级透传场景，详见官方文档 AndroidConfig.category
			    },
			  },
			  // 荣耀
			  honorField: {
			    notification: {
			      // AndroidNotification
			      clickAction: {
			        //必填，消息点击行为
			        type: 1, //必填，消息点击行为类型，取值如下：1：打开应用自定义页面 2：点击后打开特定URL 3：点击后打开应用
			        //自定义页面中intent的实现，请参见指定intent参数。当type为1时，字段intent和action至少二选一。
			        intent: `intent://com.honor.push/deeplink?#Intent;scheme=pushscheme;launchFlags=0x04000000;i.sessionType=${sessionType};S.sessionId=${sessionId};end`,
			      },
			      importance: 'NORMAL', //可选项，Android通知消息分类，决定用户设备消息通知行为，取值如下：LOW：资讯营销类消息 NORMAL：服务与通讯类消息
			    },
			  },
			  // vivo
			  vivoField: {
			    skipType: '4', //必填，点击跳转类型 1：打开APP首页 2：打开链接 3：自定义 4:打开app内指定页面，默认为1
			    skipContent: `intent:#Intent;action=com.netease.nimlib.uniapp.push.NotificationClickActivity;component=${PACKAGE_NAME}/com.netease.nimlib.uniapp.push.NotificationClickActivity;launchFlags=0x04000000;i.sessionType=${sessionType};S.sessionId=${sessionId};end`,
			    classification: '1', //可选项，消息类型 0：运营类消息，1：系统类消息。默认为0
			    category: 'IM', // 可选项，二级分类
			  },
			  // oppo
			  oppoField: {
			    channel_id: '', //可选项，指定下发的通道ID
			    category: 'IM', //可选项，通道类别名
			    notify_level: 2, //通知栏消息提醒等级，1-通知栏；2-通知栏+锁屏；16-通知栏+锁屏+横幅+震动+铃声
			    click_action_type: '1', //点击通知栏后触发的动作类型。0（默认0.启动应用；1.跳转指定应用内页（action标签名）；2.跳转网页；4.跳转指定应用内页（全路径类名）；5.跳转Intent scheme URL: "",
			    click_action_activity:
			      'com.netease.nimlib.uniapp.push.NotificationClickActivity',
			    action_parameters: JSON.stringify({
			      sessionType: sessionType,
			      sessionId: sessionId,
			    }),
			  },
			  fcmFieldV1: {
			    message: {
			      android: {
			        priority: 'high',
			        data: {
			          sessionType: sessionType,
			          sessionId: sessionId,
			        },
			        notification: {
			          click_action:
			            'com.netease.nimlib.uniapp.push.NotificationClickActivity',
			        },
			      },
			    },
			  },
				harmonyField: JSON.stringify({
					payload: {
						notification: {
							clickAction: {
								actionType: 0,
								data: {
									sessionId: "ctt5",
									sessionType: "0"
								}
							}
						},
					}
				}),
			
			  // 特别声明: 魅族暂不可用, 需要 IM 服务器支持
			
			  // IOS 通用
			  sessionId: sessionId,
			  sessionType: sessionType,
			})
			const res = await globalData.nim.V2NIMMessageService.sendMessage(message, `${senderId}|${conversationType}|${receiverId}`, {
			  pushConfig: {
			    pushEnabled: true,
			    // "pushNickEnabled": true, // 可以选择是否推送昵称
			    // "pushContent": "您有新消息请查收", // 可以指定推送内容
			    pushPayload: pushPayload
			  }
			})
		},
    refreshLog: function (type, content) {
      this.logArr.push({
        name: 'li',
        attrs: { style: `word-break: break-all; font-size: 13px; margin-bottom: 8px; color: ${type}` },
        children: [{
          type: 'text',
          text: content
        }]
      })
      if (this.logArr.length > 1000) {
        this.logArr = this.logArr.slice(-1000)
      }
    },
    changeApnsCertificate: function(event) {
      if (event) {
        this.usingP8Certificate = event.detail.value					
      } else {
        console.error('Cannot change apns certificate, event is empty')
      }
    },
    clearLog: function() {
      this.logArr = []
    },
		applyNotificationPermission: function () {
			const nimPushPlugin = uni.requireNativePlugin('NIMUniPlugin-PluginModule')
			nimPushPlugin.requestAPNSPermission()
		}
	}
}
</script>

<style>
@import url("./index.css");
</style>
