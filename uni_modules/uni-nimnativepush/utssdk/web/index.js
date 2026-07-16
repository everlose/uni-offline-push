/**
 * 获取设备推送令牌
 */
function getDeviceTokenImpl(options, callback) {
  // 使用默认值处理入参
  const pushType = options.suggestPushType || '';
  const config = options.config || {};
  
  // Web 环境不支持原生推送服务，直接返回空字符串
  console.error("Failed to get push token: Web platform does not support native push services");
  callback('');
}

function addOpenNotificationListenerImpl(callback) {
  console.error("Web platform does not support open notification listener");
}

function removeOpenNotificationListenerImpl(callback) {
  console.error("Web platform does not support open notification listener");
}

// 导出 getDeviceToken 函数，用于 uni-ext-api 的映射
export const getDeviceToken = getDeviceTokenImpl;
export const addOpenNotificationListener = addOpenNotificationListenerImpl;
export const removeOpenNotificationListener = removeOpenNotificationListenerImpl;

// 导出 NIMNativePush 命名空间
export const NIMNativePush = {
  getDeviceToken: getDeviceTokenImpl,
  addOpenNotificationListener: addOpenNotificationListenerImpl,
  removeOpenNotificationListener: removeOpenNotificationListenerImpl
};
