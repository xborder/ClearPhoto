package helder.tese.cameraexample;


public class FaceDetection {
	
    public FaceDetection(String cascadePath) {
        mNativeObj = nativeCreateObject(cascadePath);
    }

//    public void start() {
//        nativeStart(mNativeObj);
//    }
//
//    public void stop() {
//        nativeStop(mNativeObj);
//    }

    public void detect(int pw, int ph, byte[] image) {
        nativeDetect(mNativeObj, pw, ph, image);
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject(String cascadePath);
    private static native void nativeDestroyObject(long thiz);
//    private static native void nativeStart(long thiz);
//    private static native void nativeStop(long thiz);
    private static native void nativeDetect(long thiz, int pw, int ph, byte[] image);
    
}
