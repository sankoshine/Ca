package andy.ca;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
/**
 * Created by sanko on 2016/2/8.
 */
public class FaceHelper {
    static {
        System.loadLibrary("Face");
    }

    public int predictForConfidence(String parentDir){
        return predict(parentDir);
    }

    public int detectFacesCount(String parentDir, int photoId){
        return detect(parentDir, photoId);
    }
    public int trainFaces(String parentDir, int countPhoto){
        return train(parentDir, countPhoto);
    }

    private static native int train(String parentDir, int countPhoto);
    public static native int predict(String parentDir);
    private static native int detect(String parentDir, int photoId);

}
