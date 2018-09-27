package com.prosysopc.ua.simpleandroidclient;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.prosysopc.ua.UserIdentity;
import com.prosysopc.ua.client.UaClient;

import org.opcfoundation.ua.builtintypes.DataValue;
import org.opcfoundation.ua.builtintypes.DateTime;
import org.opcfoundation.ua.builtintypes.NodeId;
import org.opcfoundation.ua.builtintypes.UnsignedInteger;
import org.opcfoundation.ua.builtintypes.Variant;
import org.opcfoundation.ua.core.Attributes;
import org.opcfoundation.ua.core.Identifiers;
import org.opcfoundation.ua.transport.security.SecurityMode;

//mainactivity class start
public class MainActivity extends Activity {
    String regex = "##";
    String address = "opc.tcp://192.168.43.193";
    Button connectButton;
    Button methodButton;
    TextView textView1;
    TextView textView2;
    TextView textView3;
    TextView textView4;
    TextView textView5;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        connectButton = (Button) findViewById(R.id.connectButton);
        methodButton = (Button) findViewById(R.id.methodButton);
        textView1 = (TextView) findViewById(R.id.textView1);
        textView2 = (TextView) findViewById(R.id.textView2);
        textView3 = (TextView) findViewById(R.id.textView3);
        textView4 = (TextView) findViewById(R.id.textView4);
        textView5 = (TextView) findViewById(R.id.textView5);

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                ConnectAndReadTask task = new ConnectAndReadTask();
                task.execute();
            }

        });
        methodButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MethodTask task = new MethodTask();
                task.execute();
            }

        });

    }
    //ConnectAndReadTask class
    private class ConnectAndReadTask extends AsyncTask<Void, Void, String> {

        public ConnectAndReadTask() {
        }
        @Override
        protected String doInBackground(Void... params) {
            //UaClient object
            UaClient client = null;
            String time = null;
            String b = null;
            String g = null;
            String r = null;
            String sta = null;
            String answer = null;
            try {
                client = new UaClient(address);
                client.setTimeout(60000);
                client.setSecurityMode(SecurityMode.NONE);
                client.setUserIdentity(new UserIdentity());
                client.connect();

                DataValue dt = client.readValue(Identifiers.Server_ServerStatus_CurrentTime);
                time = ((DateTime) dt.getValue().getValue()).toString();
                DataValue blue = client.readValue(NodeId.parseNodeId("ns=1;s=blue"));
                DataValue green = client.readValue(NodeId.parseNodeId("ns=1;s=green"));
                DataValue red = client.readValue(NodeId.parseNodeId("ns=1;s=red"));
                DataValue status = client.readValue(NodeId.parseNodeId("ns=1;s=status"));
                b = blue.getValue().toString();
                g = green.getValue().toString();
                r = red.getValue().toString();
                sta = status.getValue().toString();
                b = ("Blue: " + b);
                g = ("Green: " + g);
                r = ("Red: " + r);
                sta = ("Status: " + sta);

                client.disconnect();
            } catch (Exception e) {
                time= e.toString();
                b = e.toString();
                g = e.toString();
                r = e.toString();
                sta = e.toString();
            }

            answer = time+regex+b+regex+g+regex+r+regex+sta+regex;

            return answer;
        }

        @Override
        protected void onPostExecute(String ans) {
            textView1.setText(ans.split(regex)[0].substring(0,29));
            textView2.setText(ans.split(regex)[1]);
            textView3.setText(ans.split(regex)[2]);
            textView4.setText(ans.split(regex)[3]);
            textView5.setText(ans.split(regex)[4]);
        }
    }
    //MethodTask class
    private class MethodTask extends AsyncTask<Void, Void, String> {

        public MethodTask() {
        }
        @Override
        protected String doInBackground(Void... params) {
            //UaClient object
            UaClient client = null;
            String time = null;
            String b = null;
            String g = null;
            String r = null;
            String sta = null;
            String answer = null;
            try {
                client = new UaClient(address);
                client.setTimeout(60000);
                client.setSecurityMode(SecurityMode.NONE);
                client.setUserIdentity(new UserIdentity());
                client.connect();

                DataValue dt = client.readValue(Identifiers.Server_ServerStatus_CurrentTime);
                time = ((DateTime) dt.getValue().getValue()).toString();
                Variant input = new Variant("a");
                Variant output[] = client.call(NodeId.parseNodeId("ns=1;s=ARM"), NodeId.parseNodeId("ns=1;s=switch"),input);
                DataValue blue = client.readValue(NodeId.parseNodeId("ns=1;s=blue"));
                DataValue green = client.readValue(NodeId.parseNodeId("ns=1;s=green"));
                DataValue red = client.readValue(NodeId.parseNodeId("ns=1;s=red"));
                DataValue status = client.readValue(NodeId.parseNodeId("ns=1;s=status"));
                b = blue.getValue().toString();
                g = green.getValue().toString();
                r = red.getValue().toString();
                sta = status.getValue().toString();
                b = ("Blue: " + b);
                g = ("Green: " + g);
                r = ("Red: " + r);
                sta = ("Status: " + sta);

                client.disconnect();
            } catch (Exception e) {
                time= e.toString();
                b = e.toString();
                g = e.toString();
                r = e.toString();
                sta = e.toString();
            }

            answer = time+regex+b+regex+g+regex+r+regex+sta+regex;
            return answer;
        }

        @Override
        protected void onPostExecute(String ans) {
            textView1.setText(ans.split(regex)[0].substring(0,29));
            textView2.setText(ans.split(regex)[1]);
            textView3.setText(ans.split(regex)[2]);
            textView4.setText(ans.split(regex)[3]);
            textView5.setText(ans.split(regex)[4]);
        }
    }
}
