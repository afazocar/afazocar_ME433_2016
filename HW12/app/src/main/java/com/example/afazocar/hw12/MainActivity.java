package com.example.afazocar.hw12;

// libraries
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import java.io.IOException;
import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    SeekBar     pickRow;
    TextView    pickRowText;
    SeekBar     pickThreshold;
    TextView    pickThresholdText;
    TextView    debugPrint;
    Button      startButton;
    //SeekBar     pickFlash;
    //TextView    pickFlashText;
    int         startY = 50*4; // Start tracking from row 200 (progress bar starts at 50%)
    double      greenThreshold = 0; // Start threshold at 600 (progress bar starts at 80%)
    int         flashThreshold = 75;
    //boolean     flashOn;
    boolean     startMoving = false;

    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640,480,Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;

    static long prevtime = 0; // for FPS calculation

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
        mSurfaceHolder = mSurfaceView.getHolder();

        mTextureView = (TextureView) findViewById(R.id.textureview);
        mTextureView.setSurfaceTextureListener(this);

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        paint1.setColor(0xffff00ff); // red
        paint1.setTextSize(24);

        pickRow     = (SeekBar) findViewById(R.id.pickRow);
        pickRowText = (TextView) findViewById(R.id.pickRowText);
        pickRowText.setText("Row to track: " + startY);
        setMyRowListener();

        pickThreshold       = (SeekBar) findViewById(R.id.pickThreshold);
        pickThresholdText   = (TextView) findViewById(R.id.pickThresholdText);
        pickThresholdText.setText("Threshold for green: " + greenThreshold);
        setMyThresholdListener();

        debugPrint  = (TextView) findViewById(R.id.debugPrint);

        startButton = (Button)this.findViewById(R.id.startButton);
        final MediaPlayer mp = MediaPlayer.create(this, R.raw.rocket);
        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mp.start();

                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    public void run() {
                        // Actions to do after 10.5 seconds
                        startMoving = true;
                    }
                }, 10500);

            }
        });

        /*pickFlash       = (SeekBar) findViewById(R.id.pickFlash);
        pickFlashText   = (TextView) findViewById(R.id.pickFlashText);
        if (flashThreshold >= 50) {
            pickFlashText.setText("Flash: ON");
            flashOn = true;
        }
        else {
            pickFlashText.setText("Flash: OFF");
            flashOn = false;
        }
        setMyFlashListener();*/
    }


    private void setMyRowListener() {
        pickRow.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                startY = progress*4; // Progress bar value*4 is row to track
                pickRowText.setText("Row to track: " + startY);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    private void setMyThresholdListener() {
        pickThreshold.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                greenThreshold = progress*2.5; // Progress bar value*4 is row to track
                pickThresholdText.setText("Threshold for green: " + greenThreshold);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    /*private void setMyFlashListener() {
        pickFlash.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
            //Camera.Parameters parameters = mCamera2.getParameters();

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (progress >= 50) {
                    flashOn = true;
                }
                else {
                    flashOn = false;
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }*/

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setColorEffect(Camera.Parameters.EFFECT_NONE); // black and white
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        //parameters.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);    // turn on flash
        /*if (flashOn) {
            pickFlashText.setText("Flash: ON");
            parameters.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
        }
        else {
            pickFlashText.setText("Flash: OFF");
            parameters.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
        }*/
        //parameters.setAutoWhiteBalanceLock(true);                       // turn of autowhite balancing
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {

            int[] pixels = new int[bmp.getWidth()];
            //int startY = 50; // which row in the bitmap to analyse to read
            // only look at one row in the image
            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)

            // pixels[] is the RGBA data (in black an white).
            // instead of doing center of mass on it, decide if each pixel is dark enough to consider black or white
            // then do a center of mass on the thresholded array
            int[] thresholdedPixels = new int[bmp.getWidth()];
            int wbTotal = 0; // total mass
            int wbCOM = 0; // total (mass time position)
            for (int i = 0; i < bmp.getWidth(); i++) {
                // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                // if it is greater than some value (600 here), consider it black
                // play with the 600 value if you are having issues reliably seeing the line
                if (green(pixels[i]) > greenThreshold) {
                    thresholdedPixels[i] = 0;
                }
                else {
                    thresholdedPixels[i] = 255;
                }

                /*if (255*3-(red(pixels[i])+green(pixels[i])+blue(pixels[i])) > blackThreshold) {
                    thresholdedPixels[i] = 255*3;
                }
                else {
                    thresholdedPixels[i] = 0;
                }*/
                wbTotal = wbTotal + thresholdedPixels[i];
                wbCOM = wbCOM + thresholdedPixels[i]*i;
            }
            int COM;
            //watch out for divide by 0
            if (wbTotal<=0) {
                COM = bmp.getWidth()/2;
            }
            else {
                COM = wbCOM/wbTotal;
            }

            // draw a circle where you think the COM is
            canvas.drawCircle(COM, startY, 5, paint1);

            // also write the value as text
            canvas.drawText("COM = " + COM, 10, 200, paint1);
            c.drawBitmap(bmp, 0, 0, null);
            mSurfaceHolder.unlockCanvasAndPost(c);

            // calculate the FPS to see how fast the code is running
            long nowtime = System.currentTimeMillis();
            long diff = nowtime - prevtime;
            mTextView.setText("FPS " + 1000/diff);
            prevtime = nowtime;


            if (startMoving) {
                if (COM < bmp.getWidth() / 2) {
                    debugPrint.setText("TURN LEFT");
                } else if (COM > bmp.getWidth() / 2) {
                    debugPrint.setText("TURN RIGHT");
                } else {
                    debugPrint.setText("GO STRAIGHT");
                }
            }
        }
    }
}