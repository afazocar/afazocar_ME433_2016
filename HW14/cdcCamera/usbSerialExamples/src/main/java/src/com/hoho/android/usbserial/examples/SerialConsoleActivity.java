/* Copyright 2011-2013 Google Inc.
 * Copyright 2013 mike wakerly <opensource@hoho.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: https://github.com/mik3y/usb-serial-for-android
 */

package com.hoho.android.usbserial.examples;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.util.HexDump;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

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
import static android.graphics.Color.rgb;

/**
 * Monitors a single {@link UsbSerialPort} instance, showing all data
 * received.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public class SerialConsoleActivity extends Activity implements TextureView.SurfaceTextureListener {

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // CAMERA STUFF
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
    boolean     checkColors = true;

    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640,480,Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;

    static long prevtime = 0; // for FPS calculation
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    private final String TAG = SerialConsoleActivity.class.getSimpleName();

    /**
     * Driver instance, passed in statically via
     * {@link #show(Context, UsbSerialPort)}.
     *
     * <p/>
     * This is a devious hack; it'd be cleaner to re-create the driver using
     * arguments passed in with the {@link #startActivity(Intent)} intent. We
     * can get away with it because both activities will run in the same
     * process, and this is a simple demo.
     */
    private static UsbSerialPort sPort = null;

    private TextView mTitleTextView;
    private TextView mDumpTextView;
    private ScrollView mScrollView;
    private CheckBox chkDTR;
    private CheckBox chkRTS;

    private TextView picReturnTitle;

    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();

    private SerialInputOutputManager mSerialIoManager;

    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {

        @Override
        public void onRunError(Exception e) {
            Log.d(TAG, "Runner stopped.");
        }

        @Override
        public void onNewData(final byte[] data) {
            SerialConsoleActivity.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    SerialConsoleActivity.this.updateReceivedData(data);
                }
            });
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.serial_console);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // CAMERA STUFF
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
                checkColors = false;
                mp.start();

                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    public void run() {
                        // Actions to do after 10.5 seconds
                        startMoving = true;
                    }
                }, 0);//10500);

            }
        });
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        mTitleTextView = (TextView) findViewById(R.id.demoTitle);
        mDumpTextView = (TextView) findViewById(R.id.consoleText);
        mScrollView = (ScrollView) findViewById(R.id.demoScroller);
        chkDTR = (CheckBox) findViewById(R.id.checkBoxDTR);
        chkRTS = (CheckBox) findViewById(R.id.checkBoxRTS);

        // LED BRIGHTNESS BAR INSTANTIATION
        picReturnTitle = (TextView) findViewById(R.id.picReturnTitle);
        picReturnTitle.setText("PIC Return: ");

        chkDTR.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                try {
                    sPort.setDTR(isChecked);
                }catch (IOException x){}
            }
        });

        chkRTS.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                try {
                    sPort.setRTS(isChecked);
                }catch (IOException x){}
            }
        });

    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // CAMERA STUFF
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
                greenThreshold = progress*5;//2.5; // Progress bar value*4 is row to track
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
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    protected void onPause() {
        super.onPause();
        stopIoManager();
        if (sPort != null) {
            try {
                sPort.close();
            } catch (IOException e) {
                // Ignore.
            }
            sPort = null;
        }
        finish();
    }

    void showStatus(TextView theTextView, String theLabel, boolean theValue){
        String msg = theLabel + ": " + (theValue ? "enabled" : "disabled") + "\n";
        theTextView.append(msg);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "Resumed, port=" + sPort);
        if (sPort == null) {
            mTitleTextView.setText("No serial device.");
        } else {
            final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

            UsbDeviceConnection connection = usbManager.openDevice(sPort.getDriver().getDevice());
            if (connection == null) {
                mTitleTextView.setText("Opening device failed");
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);

                showStatus(mDumpTextView, "CD  - Carrier Detect", sPort.getCD());
                showStatus(mDumpTextView, "CTS - Clear To Send", sPort.getCTS());
                showStatus(mDumpTextView, "DSR - Data Set Ready", sPort.getDSR());
                showStatus(mDumpTextView, "DTR - Data Terminal Ready", sPort.getDTR());
                showStatus(mDumpTextView, "DSR - Data Set Ready", sPort.getDSR());
                showStatus(mDumpTextView, "RI  - Ring Indicator", sPort.getRI());
                showStatus(mDumpTextView, "RTS - Request To Send", sPort.getRTS());

            } catch (IOException e) {
                Log.e(TAG, "Error setting up device: " + e.getMessage(), e);
                mTitleTextView.setText("Error opening device: " + e.getMessage());
                try {
                    sPort.close();
                } catch (IOException e2) {
                    // Ignore.
                }
                sPort = null;
                return;
            }
            mTitleTextView.setText("Serial device: " + sPort.getClass().getSimpleName());
        }
        onDeviceStateChange();
    }

    private void stopIoManager() {
        if (mSerialIoManager != null) {
            Log.i(TAG, "Stopping io manager ..");
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if (sPort != null) {
            Log.i(TAG, "Starting io manager ..");
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange() {
        stopIoManager();
        startIoManager();
    }

    private void updateReceivedData(byte[] data) {
        final String message = "Read " + data.length + " bytes: \n"
                + HexDump.dumpHexString(data) + "\n\n";

        String picReturn = new String(data);

        picReturnTitle.setText("PIC Return: " + picReturn);

        //mDumpTextView.append(message);
        //mScrollView.smoothScrollTo(0, mDumpTextView.getBottom());
        //byte[] sData = {'a',0}; try { sPort.write(sData, 10); } catch (IOException e) { }

        /*int i = 1; int j = 2; // the two numbers to send
        String sendString = String.valueOf(i) + " " + String.valueOf(j);
        try {
            sPort.write(sendString.getBytes(),10); // 10 is the timeout (error)
        }
        catch (IOException e) {}*/


    }

    /**
     * Starts the activity, using the supplied driver instance.
     *
     * @param context
     * @param driver
     */
    static void show(Context context, UsbSerialPort port) {
        sPort = port;
        final Intent intent = new Intent(context, SerialConsoleActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        context.startActivity(intent);
    }

    @Override
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

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {

            int[] pixels = new int[bmp.getWidth()];
            int[] thresholdedPixels = new int[bmp.getWidth()];
            int wbTotal = 0;    // total mass
            int wbCOM = 0;      // weighted mass (mass*position)

            int[] thresholdedColors = new int[bmp.getWidth()];

            if (checkColors) { // Overlay green/black to see what camera sees
                for (int row = 15; row < bmp.getHeight() - 15; row++) {
                    bmp.getPixels(pixels, 0, bmp.getWidth(), 0, row, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)
                    for (int i = 0; i < bmp.getWidth(); i++) {
                        if (255 - (green(pixels[i]) - red(pixels[i])) > greenThreshold) { // not green
                            thresholdedPixels[i] = 255;
                            thresholdedColors[i] = rgb(0, 0, 0);
                        } else { // green
                            thresholdedPixels[i] = 0;
                            thresholdedColors[i] = rgb(0, 255, 0);

                        }

                        if (row == startY) { // calculate COM at desired row
                            wbTotal = wbTotal + thresholdedPixels[i];
                            wbCOM = wbCOM + thresholdedPixels[i] * i;
                        }
                    }
                    bmp.setPixels(thresholdedColors, 0, bmp.getWidth(), 0, row, bmp.getWidth(), 1);
                }
            } else {    // Turn off overlay when launch button is pressed
                bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)
                for (int i = 0; i < bmp.getWidth(); i++) {
                    if (255 - (green(pixels[i]) - red(pixels[i])) > greenThreshold) { // not green
                        thresholdedPixels[i] = 255;
                        thresholdedColors[i] = rgb(0, 0, 0);
                    } else { // green
                        thresholdedPixels[i] = 0;
                        thresholdedColors[i] = rgb(0, 255, 0);
                    }
                    wbTotal = wbTotal + thresholdedPixels[i];
                    wbCOM = wbCOM + thresholdedPixels[i] * i;
                }
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
            c.drawBitmap(bmp, 0, 0, null);
            mSurfaceHolder.unlockCanvasAndPost(c);

            // calculate the FPS to see how fast the code is running
            long nowtime = System.currentTimeMillis();
            long diff = nowtime - prevtime;
            mTextView.setText("FPS " + 1000/diff);
            prevtime = nowtime;


            if (startMoving) {
                if (COM < bmp.getWidth() / 3) {
                    debugPrint.setText("TURN HARD LEFT");
                    String sendString = String.valueOf(4);
                    sendString = sendString + "\n";
                    try {
                        sPort.write(sendString.getBytes(), 10); // 10 is the timeout (error)
                    } catch (IOException e) {}
                } else if (COM > bmp.getWidth() * 2 / 3) {
                    debugPrint.setText("TURN HARD RIGHT");
                    String sendString = String.valueOf(5);
                    sendString = sendString + "\n";
                    try {
                        sPort.write(sendString.getBytes(), 10); // 10 is the timeout (error)
                    } catch (IOException e) {}
                } else if (COM < bmp.getWidth() / 2) {
                    debugPrint.setText("TURN LEFT");
                    String sendString = String.valueOf(2);
                    sendString = sendString + "\n";
                    try {
                        sPort.write(sendString.getBytes(),10); // 10 is the timeout (error)
                    }
                    catch (IOException e) {}
                } else if (COM > bmp.getWidth() / 2) {
                    debugPrint.setText("TURN RIGHT");
                    String sendString = String.valueOf(3);
                    sendString = sendString + "\n";
                    try {
                        sPort.write(sendString.getBytes(),10); // 10 is the timeout (error)
                    }
                    catch (IOException e) {}
                } /*else {
                    debugPrint.setText("GO STRAIGHT");
                    String sendString = String.valueOf(1);
                    sendString = sendString + "\n";
                    try {
                        sPort.write(sendString.getBytes(),10); // 10 is the timeout (error)
                    }
                    catch (IOException e) {}
                }*/
            }
            else {
                String sendString = String.valueOf(0);
                sendString = sendString + "\n";
                try {
                    sPort.write(sendString.getBytes(),10); // 10 is the timeout (error)
                }
                catch (IOException e) {}
            }
        }
    }
}
