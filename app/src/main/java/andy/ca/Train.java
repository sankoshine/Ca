package andy.ca;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.io.File;

public class Train extends AppCompatActivity {

    private static final String PARENT_DIR = "/storage/emulated/0/0000/";
    private static final int CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE = 100;
    private static final String TAG = "Andy/Train";
    private Button takePhoto = null;
    private Button train = null;
    private ImageView img1,img2,img3,img4;
    private TextView text1,text2,text3,text4;
    private GridView gridView;
    FaceHelper fc = null;
    RelativeLayout loadingPanel;
    private static int no_of_faces = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_train);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        fc  = new FaceHelper();

        loadingPanel = (RelativeLayout) findViewById(R.id.loadingPanel);

        img1 = (ImageView) findViewById(R.id.image1);
        text1 = (TextView) findViewById(R.id.textView1);
        img2 = (ImageView) findViewById(R.id.image2);
        text2 = (TextView) findViewById(R.id.textView2);
        img3 = (ImageView) findViewById(R.id.image3);
        text3 = (TextView) findViewById(R.id.textView3);
        img4 = (ImageView) findViewById(R.id.image4);
        text4 = (TextView) findViewById(R.id.textView4);

        train = (Button)findViewById(R.id.train);
        train.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Log.d(TAG, "run: in");
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                loadingPanel.setVisibility(View.VISIBLE);
                            }
                        });
                        train();
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                loadingPanel.setVisibility(View.GONE);
                            }
                        });
                        Log.d(TAG, "run: out");
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

    public void takePhotoListener(View v) {
        Log.d(TAG, "onClick "+v.getId());
        takePhoto(v.getId()%4+1);
    }

    private void takePhoto(int photoId){
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        File file = new File(PARENT_DIR + photoId + ".jpg");
        Uri uri = Uri.fromFile(file);
        Log.d(TAG, uri.toString());
        intent.putExtra(MediaStore.EXTRA_OUTPUT, uri);
        startActivityForResult(intent, photoId);
    }

    private int train(){
        return fc.train(PARENT_DIR, 4);
    }

    private int detect(final int photoId){
        new Thread(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "run: start detect");
                no_of_faces = fc.detect(PARENT_DIR, photoId);
                Log.d(TAG, "run: no of faces - "+no_of_faces);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        String face_detect_result = "请重新拍摄！";
                        if(no_of_faces==1)face_detect_result = "图像合格";
                        Log.d(TAG, "run: face detect result - "+face_detect_result);
                        switch (photoId){
                            case 1:text1.setText(face_detect_result);break;
                            case 2:text2.setText(face_detect_result);break;
                            case 3:text3.setText(face_detect_result);break;
                            case 4:text4.setText(face_detect_result);break;
                        }
                    }
                });
            }
        }).start();
        return 0;
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(1<=requestCode && requestCode<=4) {
            if (resultCode == RESULT_OK) {
                Bitmap bmp = null;
                Log.d(TAG, "onActivityResult: IMAGE OK - " + requestCode);
                bmp = BitmapFactory.decodeFile(PARENT_DIR + requestCode + ".jpg");
                switch (requestCode) {
                    case 1:
                        img1.setImageBitmap(bmp);
                        text1.setText("Checking");
                        break;
                    case 2:
                        img2.setImageBitmap(bmp);
                        text2.setText("Checking");
                        break;
                    case 3:
                        img3.setImageBitmap(bmp);
                        text3.setText("Checking");
                        break;
                    case 4:
                        img4.setImageBitmap(bmp);
                        text4.setText("Checking");
                        break;
                }
                detect(requestCode);
            } else if (resultCode == RESULT_CANCELED) {
                Log.d(TAG, "onActivityResult: CANCELED");
            } else {
                Log.e(TAG, "onActivityResult: ERR " + resultCode);
            }
        }
    }

}

