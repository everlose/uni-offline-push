<script>
// #ifdef APP-PLUS
const nimPushPlugin = uni.requireNativePlugin('NIMUniPlugin-PluginModule')
console.log('~~~~~~~~~~nimPushPlugin', nimPushPlugin)
console.log('~~~~~~~~~~nimPushPlugin.addEventListener', nimPushPlugin.addEventListener)
console.log('~~~~~~~~~~nimPushPlugin.addOpenNotificationListener', nimPushPlugin.addOpenNotificationListener)
console.log('~~~~~~~~~~nimPushPlugin.getDeviceToken', nimPushPlugin.getDeviceToken)
console.log('~~~~~~~~~~nimPushPlugin.requestAPNSPermission', nimPushPlugin.requestAPNSPermission)
// #endif

function getGlobalData() {
  const app = getApp()
  if (!app) return null
  app.globalData = app.globalData || {}
  return app.globalData
}

function handleOpenNotification(result) {
	// result aos 真实调通返回值
	// {
	//     "mipush_notified": "true",
	//     "pushTargetComponent": "true",
	//     "sessionId": "cs1",
	//     "messageId": "smm6490575310016323890",
	//     "eventMessageType": "1000",
	//     "sessionType": "0",
	//     "key_message": "messageId={smm6490575310016323890},passThrough={0},alias={null},topic={null},userAccount={null},content={},description={您有一条新消息4},title={推图},isNotified={true},notifyId={1889865864},notifyType={0}, category={null}, extra={{intent_uri=intent:#Intent;action=com.netease.nimlib.uniapp.push.NotificationClickActivity;component=com.netease.nim.demo/com.netease.nimlib.uniapp.push.NotificationClickActivity;launchFlags=0x04000000;i.sessionType=0;S.sessionId=cs1;end, channel_name=运营消息, channel_description=, n_stats_expose=ZfHxi8YUPz2tMA0tuvrHDMH/UfeEEl2E3lA5zDRoToQu8qCo1lJe61xHMFmOkqYc/haJGlFGvuloRBA6G27gNo642A64gjrKF2q/5GPWVyWtNZh/wti9vi9hgAQtOJCb, _msg_pri=20, rights_stat=100000000, simplify_pull_type=2, notify_foreground=0, section_prr_cl=0, use_clicked_activity=true, timeout=43200, pushTitle=推图, __target_name=G681QTcx4G0LjpfJCUfMRTeeSxuRVDpOW1xnmO8kGha5d3Xg5muKzlp844JcV/6V, nim=1, _gray_pri_t_p=20, fe_ts=1753100163238, notify_effect=2, __m_ts=1753100163249}}"
	// }
	if (typeof result === 'object') {
		console.log("=====addOpenNotificationListener success:", JSON.stringify(result))
		uni.navigateTo({
		  url: `/pages/index/index?sessionId=${result.sessionId}&sessionType=${result.sessionType}`, // 通过 URL 传参
		  success: () => console.log('跳转成功'),
		  fail: (err) => console.error('跳转失败', err)
		});
	} else {
		console.log("=====addOpenNotificationListener unexpected:", result)
	}
}

export default {
  onLaunch: function() {
    console.log('App Launch')
    // #ifdef APP-PLUS
		nimPushPlugin.addOpenNotificationListener(handleOpenNotification);
		// #endif
		// #ifdef APP-HARMONY
		uni.NIMNativePush.addOpenNotificationListener(handleOpenNotification);
		// #endif

    // #ifndef APP-HARMONY
    uni.getBatteryInfo({
      success(res) {
        console.log(res);
        uni.showToast({
          title: "当前电量：" + res.level + '%',
          icon: 'none'
        });
      }
    })
    // #endif
  },
  onShow: function() {
    const globalData = getGlobalData()
    // app && app.globalData.nim && app.globalData.nim.offlinePush.updateAppBackground({
    //   isBackground: false
    // })
    globalData && globalData.nim && globalData.nim.V2NIMSettingService.setAppBackground(false)
    if (globalData) globalData.appHide = false
    console.log('App Show')
  },
  onHide: function() {
    const globalData = getGlobalData()
    // app && app.globalData.nim && app.globalData.nim.offlinePush.updateAppBackground({
    //   isBackground: true
    // })
    globalData && globalData.nim && globalData.nim.V2NIMSettingService.setAppBackground(false)
    if (globalData) globalData.appHide = true
    console.log('App Hide')
  },
}
</script>

<style>
	/*每个页面公共css */
</style>
