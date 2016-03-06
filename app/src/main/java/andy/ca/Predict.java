package andy.ca;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;

public class Predict extends AppCompatActivity {
    private static final String PARENT_DIR = "/storage/emulated/0/1000/";
    private static final int CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE = 100;
    private static final String TAG = "Andy/Predict";
    private Button takePhoto = null;
    private Button predict = null;
    FaceHelper fc = null;
    ImageView img = null;
    TextView txt = null;
    Bitmap bmp;
    String face_detect_result;
    int confidence;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_predict);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        fc = new FaceHelper();
        img = (ImageView) findViewById(R.id.imageView);
        txt = (TextView) findViewById(R.id.textView);
        takePhoto = (Button) findViewById(R.id.predictPhoto);
        takePhoto.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                takePhoto("9");
            }
        });

        predict = (Button) findViewById(R.id.predict);
        predict.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                txt.setText("正在签到");
                            }
                        });
                        confidence = fc.predictForConfidence(PARENT_DIR);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Log.d(TAG, "run: confidence is "+confidence);
                                txt.setText(Integer.toString(confidence));
                            }
                        });
                    }
                }).start();
            }
        });

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode==CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE) {
            if (resultCode == RESULT_OK) {
                Log.d(TAG, "onActivityResult: IMAGE OK - " + requestCode);
                bmp = BitmapFactory.decodeFile(PARENT_DIR + "9.jpg");
                Log.d(TAG, "onActivityResult: IMAGE LOADING");
                img.setImageBitmap(bmp);
                detect();
            } else if (resultCode == RESULT_CANCELED) {
                Log.d(TAG, "onActivityResult: CANCELED");
            } else {
                Log.e(TAG, "onActivityResult: ERR " + resultCode);
            }
        }
    }

    private void detect(){
        Log.d(TAG, "detect: in");
        new Thread(new Runnable() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        txt.setText("Checking");
                        Log.d(TAG, "run: set text 'checking'");
                    }
                });
                int no_of_faces = fc.detectFacesCount(PARENT_DIR, 9);
                Log.d(TAG, "run: number of face is "+no_of_faces);
                face_detect_result = "请重新拍摄！";
                if(no_of_faces==1)face_detect_result = "图像合格";
                Log.d(TAG, "run: face detect result - " + face_detect_result);
                bmp = BitmapFactory.decodeFile(PARENT_DIR + "9_face.jpg");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        txt.setText(face_detect_result);
                        img.setImageBitmap(bmp);
                    }
                });
            }
        }).start();
    }

    private void takePhoto(String photoId){
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        File file = new File(PARENT_DIR + photoId + ".jpg");
        Uri uri = Uri.fromFile(file);
        Log.d(TAG, uri.toString());
        intent.putExtra(MediaStore.EXTRA_OUTPUT, uri);
        startActivityForResult(intent, CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE);
    }
}
