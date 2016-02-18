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
    public static native int Find(String imageName,String FileName,String Csv);
    public static native int Recog(String imageName,String Filename);
}
