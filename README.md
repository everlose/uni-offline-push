## 启动

安装依赖

```bash
npm install
```

UniApp推送Demo

## 安装依赖

npm install

## 启动项目

1. 注意去 pages/index/index.vue 修改成自己的登录信息和推送配置

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
  // 1.1.5 后新加的 ios pushkit 证书传递
	pushkit: {
		certificateName: '*********'
	},
	// 鸿蒙的配置
	harmonyPush: {
		certificateName: '*********'
	}
}
```

2. “运行” -> "运行到 Android 基座" -> “使用自定义基座”


## 注意事项

1. 截止 25/07/23 日前推送插件版本最新为 1.1.5, 可以检查云信官网看看有没有更新的 [实现 uni-app 离线推送](https://doc.yunxin.163.com/messaging2/guide/zc4MTg5MDY?platform=client#%E7%AC%AC%E4%BA%8C%E6%AD%A5%E8%AE%BE%E7%BD%AE-uni-app-%E5%B7%A5%E7%A8%8B)
2. 本工程使用前需要制作一份自定义基座. 打基座的过程中会因为 uniapp 应用标识时 __UNI__B380AC4, 和开发者本人没有关联而失败, 所以建议开发者只是搬运这个工程的代码到新工程后后去打基座.
3. 本工程已经引入了 nativeplugins, 并且在 manifest.json 里开启了推送配置, 请开发者参照
4. 重点参照 App.vue 的推送拉起事件 addOpenNotificationListener 监听, 和 pages/index/index.vue 中的注入插件 setOfflinePushConfig 函数, 和 sendMessage 发送消息逻辑
5. 工程里的具体的 IM 账号, 证书都需要开发者替换.

此外推送还有其他注意事项

4. 华为注册厂商推送服务时，包含 agconnect-services.json 文件。您需要下载该文件，并将其放至您的 uni-app 应用根目录下的 nativeplugins/NIMUniPlugin/android/assets 文件夹下。
5. 荣耀注册厂商推送服务时，包含 mcs-services.json 文件。您需下载该文件，并将其放至您的 uni-app 应用根目录下的 nativeplugins/NIMUniPlugin/android/assets 文件夹下。

## FCM 推送注意

插件版本 >= 1.1.4 且开发者需要自行得到一份 google-services.json 并放到目录 nativeplugins/android/assets/ 下


## iOS 推送配置

自 `NIMUniPlugin 1.1.5` 起，iOS 端开始支持 **VoIP 推送**，并支持**通知权限后置申请**（即不在 App 启动时立即弹出授权弹窗，而由业务方在合适的时机主动调用）。Demo 已默认启用这两项能力，关键配置都集中在 `manifest.json` -> `app-plus` -> `distribute` -> `ios` 节点下：

```jsonc
"ios" : {
    "dSYMs" : false,
    "idfa" : false,
    // 关键：手动申请通知权限，不在启动时自动请求
    "pushRegisterMode" : "manual",
    // https://ask.dcloud.net.cn/article/36430
    // voip 推送所需的后台模式
    "UIBackgroundModes" : [ "voip", "remote-notification" ]
}
```

字段说明：

1. **`pushRegisterMode: "manual"`** —— 关闭 HBuilder 默认的「启动即请求通知权限」行为，将权限申请时机交给业务方控制。配合插件提供的 `nimPushPlugin.requestAPNSPermission()`，可以实现「用户进入会话/点击按钮时再弹授权」的后置申请方案，避免冷启动就打扰用户、影响转化率。
2. **`UIBackgroundModes: ["voip", "remote-notification"]`** —— 同时声明 `voip` 与 `remote-notification` 两个后台模式：
   - `remote-notification` 是普通 APNs 远程推送必备；
   - `voip` 是 1.1.5 插件启用 VoIP 推送通道所必需，否则系统不会将 VoIP Push 投递到 App。
3. **`apns.certificateName`** —— 在 `OFFLINE_PUSH_CONFIG.apns.certificateName`（见 `pages/index/index.vue`）中填写云信后台配置的 APNs 证书名；Demo 提供 `usingP8Certificate` 开关（页面顶部 switch），可在 p8 / p12 证书之间切换测试。

### 权限后置申请示例

Demo 中权限申请由 `pages/index/index.vue` 的 `applyNotificationPermission` 触发（页面上的「IOS 申请通知权限」按钮），不会随 App 启动自动执行：

```js
applyNotificationPermission: function () {
  const nimPushPlugin = uni.requireNativePlugin('NIMUniPlugin-PluginModule')
  nimPushPlugin.requestAPNSPermission()
}
```

业务方应在前置引导页或用户进入目标会话后再调用该方法，避免一打开 App 就弹出系统授权弹窗。

### 注意事项

1. VoIP 推送要求插件版本 `>= 1.1.5`，低版本不具备该通道，仅走普通 APNs。
2. 启用 VoIP 后，请确认在云信后台已上传与 APNs 证书同环境（开发/生产）对应的证书，并保持 `UIBackgroundModes` 中包含 `voip`。
3. `pushRegisterMode` 仅控制通知权限弹窗时机，不影响 token 的注册与上报；token 仍由插件在初始化时按 `pushType` 自动获取并上报云信。
4. **`getDeviceToken` 无回调排查**：当同时配置了 `apns.certificateName` 与 `pushkit.certificateName`（见 `OFFLINE_PUSH_CONFIG` 中的 `apns` / `pushkit` 两段），若 `getDeviceToken` 始终没有返回，可先尝试**注释掉 `pushkit.certificateName`**，看 APNs token 是否能正常回调。原因是插件内部实现为：**两个证书都传递时，需要两端 token 都获取成功才会触发回调**，只要其中一方获取异常（证书名错、证书未上传云信后台、环境不匹配等），另一方的 token 也不会返回。确认 APNs 通道正常后，再回头单独排查 PushKit 证书与 VoIP 通道。


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
