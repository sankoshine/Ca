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
    public static native int[] gray(int[] buf, int w, int h);
}
