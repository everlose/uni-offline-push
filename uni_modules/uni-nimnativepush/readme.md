# uni-nimnativepush

`uni-nimnativepush` 是一个面向云信 IM uni-app 项目的 HarmonyOS 推送辅助 UTS 插件。

当前插件仅支持 **App Harmony** 平台，用于：

- 获取 HarmonyOS Push Kit 设备 token
- 监听用户点击鸿蒙通知拉起应用时携带的参数

不支持 Android、iOS、Web、小程序等平台。Android/iOS 厂商推送请继续使用原生插件或对应平台已有实现。

## API

### `uni.NIMNativePush.getDeviceToken(options, callback)`

获取 HarmonyOS Push Kit token。

```js
uni.NIMNativePush.getDeviceToken({}, (token) => {
  if (token) {
    console.log('Harmony push token:', token)
  } else {
    console.error('Harmony push token is empty')
  }
})
```

通常不需要业务直接调用。云信 SDK 初始化后，通过 `V2NIMSettingService.setOfflinePushConfig` 传入本插件，SDK 会自动获取并上报 token。

```js
nim.V2NIMSettingService.setOfflinePushConfig(
  uni.NIMNativePush,
  {
    harmonyPush: {
      certificateName: 'DEMO_HMOS_PUSH'
    }
  }
)
```

### `uni.NIMNativePush.addOpenNotificationListener(callback)`

监听用户点击鸿蒙通知打开应用事件。

```js
uni.NIMNativePush.addOpenNotificationListener((result) => {
  console.log('open notification:', JSON.stringify(result))

  const { sessionId, sessionType } = result
  if (sessionId != null && sessionType != null) {
    uni.navigateTo({
      url: `/pages/index/index?sessionId=${sessionId}&sessionType=${sessionType}`
    })
  }
})
```

插件会解析 HarmonyOS Push Kit 点击回调中的 `clickAction.data`，并把其中字段平铺到回调结果顶层。例如 `clickAction.data.sessionId` 会变成 `result.sessionId`。

如果回调只在某个页面生命周期内有效，可以在页面卸载时移除监听，避免重复导航：

```js
function handleOpenNotification(result) {
  console.log('open notification:', JSON.stringify(result))
}

uni.NIMNativePush.addOpenNotificationListener(handleOpenNotification)
uni.NIMNativePush.removeOpenNotificationListener(handleOpenNotification)
```

## 云信发送端参数

云信 IM 服务端会读取 `pushConfig.pushPayload` 里的 `harmonyField` 字段，并将其作为华为 Push Kit `messages:send` 接口请求体的一部分。

注意：`harmonyField` 的值需要是字符串形式的 JSON，因此发送端需要双层 `JSON.stringify`。

推荐的最小参数如下：

```js
const harmonyField = JSON.stringify({
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
})

window.nim.V2NIMMessageService.sendMessage(
  window.message,
  'YOUR_CONVERSATION_ID',
  {
    pushConfig: {
      pushEnabled: true,
      pushPayload: JSON.stringify({ harmonyField })
    }
  }
)
```

不要把 `sessionId`、`sessionType` 放在 `payload.notification` 根节点下；HarmonyOS Push Kit 不会把这些未知字段作为点击参数返回。

如果需要点击通知后拿到业务参数，应放在：

```js
payload.notification.clickAction.data
```

插件会把该对象里的字段合并到 `addOpenNotificationListener` 的回调结果顶层。

## 可选字段

以下字段与点击跳转参数无关，可以按业务需要选择是否传入：

- `payload.notification.title`
- `payload.notification.body`
- `payload.notification.notifyId`
- `payload.notification.category`
- `pushOptions.ttl`
- `pushOptions.testMessage`

`category` 可能会被 HarmonyOS Push Kit 按应用权限降级，例如未申请自分类权益时可能仍显示为 `MARKETING`。不要用 `category` 是否变化判断点击参数是否生效；应以 `clickAction.data` 是否出现在回调日志中为准。

## 平台限制

当前源码只实现了：

```text
utssdk/app-harmony/index.uts
```

因此仅 App Harmony 可用。其它平台目录即使存在占位文件，也不代表已有完整实现。
