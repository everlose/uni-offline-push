## 启动

安装依赖

```bash
npm install
```

UniApp推送Demo

## 安装依赖

npm install

## 启动项目

注意去 pages/index/index.vue 修改成自己的登录信息和推送配置

```
const YOUR_ACCOUNT = '*********'
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
	// 鸿蒙的配置
	harmonyPush: {
		certificateName: '*********'
	}
}
```

“运行” -> "运行到 Android 基座" -> “使用自定义基座”


## 注意事项

1. 截止 25/07/23 日前推送插件版本最新为 1.1.3, 可以检查云信官网看看有没有更新的 [实现 uni-app 离线推送](https://doc.yunxin.163.com/messaging2/guide/zc4MTg5MDY?platform=client#%E7%AC%AC%E4%BA%8C%E6%AD%A5%E8%AE%BE%E7%BD%AE-uni-app-%E5%B7%A5%E7%A8%8B)
2. 本工程使用前需要制作一份自定义基座. 打基座的过程中会因为 uniapp 应用标识时 __UNI__B380AC4, 和开发者本人没有关联而失败, 所以建议开发者只是搬运这个工程的代码到新工程后后去打基座.
3. 本工程已经引入了 nativeplugins, 并且在 manifest.json 里开启了推送配置, 请开发者参照
4. 重点参照 App.vue 的推送拉起事件 addOpenNotificationListener 监听, 和 pages/index/index.vue 中的注入插件 setOfflinePushConfig 函数, 和 sendMessage 发送消息逻辑
5. 工程里的具体的 IM 账号, 证书都需要开发者替换.

此外推送还有其他注意事项

4. 华为注册厂商推送服务时，包含 agconnect-services.json 文件。您需要下载该文件，并将其放至您的 uni-app 应用根目录下的 nativeplugins/NIMUniPlugin/android/assets 文件夹下。
5. 荣耀注册厂商推送服务时，包含 mcs-services.json 文件。您需下载该文件，并将其放至您的 uni-app 应用根目录下的 nativeplugins/NIMUniPlugin/android/assets 文件夹下。

## FCM 推送注意

插件版本 >= 1.1.4 且开发者需要自行得到一份 google-services.json 并放到目录 nativeplugins/android/assets/ 下


## 鸿蒙推送配置

[uniapp 编鸿蒙推送说明](https://docs.popo.netease.com/team/pc/MMC/pageDetail/97a23fc297ee4a228c17e6cae161cff1?popo_locale=zh&xyz=1770086600402&appVersion=4.27.0&deviceType=4&popolocale=zh-CN&popo_hidenativebar=1&popo_noindicator=1&disposable_login_token=1&xyz=1779884845373#edit)


去 manifest.json 修改, 视图界面里需要选择 certificates 文件夹下的同名的证书.

```
    "app-harmony" : {
        "distribute" : {
            "bundleName" : "*********",
            "signingConfigs" : {
                "default" : {
                    "certpath" : "*********",
                    "keyAlias" : "*********",
                    "keyPassword" : "*********",
                    "profile" : "*********",
                    "signAlg" : "SHA256withECDSA",
                    "storeFile" : "*********",
                    "storePassword" : "*********"
                }
            }
        }
    }
```

收不到鸿蒙推送的排查思路, 的确发了消息了, 但是推送没有收到, 还可能是什么原因呢.

1. 确认鸿蒙手机鸿蒙系统
2. 线上环境 appkey: *********
3. 明确看到了 token 获取到了, 且发了 `34_1, sendCmd: 34_1,v2SetDeviceToken,ser:6 {"SER":6,"SID":34,"CID":1,"Q":[{"t":"String","v":"*********"},{"t":"String","v":"*********"},{"t":"Int","v":0}]}`
4. 登录账号为 *********, 这个账号没有其他人登录
5. 发送消息的账号为 *********, 发的时候的确携带了 pushEnable 和 pushPayload, 发的上行

```js
const harmonyField = JSON.stringify({
	payload: {
		notification: {
			clickAction: {
				actionType: 0,
				data: {
					sessionId: "*********",
					sessionType: "0"
				}
			}
		},
	}
})

window.nim.V2NIMMessageService.sendMessage(window.message, "YOUR_CONVERSATION_ID", {
	pushConfig: {
		pushEnabled: true,
		pushPayload: JSON.stringify({ harmonyField })
	}
})
```

6. 手机设置-通知和状态栏-该 app 的通知管理确认开启.
