package ufu.biolab.italo.idbracelet;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Set;

import static android.icu.lang.UCharacter.GraphemeClusterBreak.T;

public class MainActivity extends AppCompatActivity {
    Button btn_bt_turn_on,btn_bt_turn_off,btn_bt_list_devices, btn_bt_get_visible;
    private BluetoothAdapter bt_adapter;
    private Set<BluetoothDevice> pairedDevices;
    ListView lv_devices;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btn_bt_turn_on = (Button) findViewById(R.id.btn_turn_on);
        btn_bt_turn_off = (Button) findViewById(R.id.btn_turn_off);
        btn_bt_list_devices = (Button) findViewById(R.id.btn_list_devices);
        btn_bt_get_visible = (Button) findViewById(R.id.btn_get_visible);
        
        bt_adapter = BluetoothAdapter.getDefaultAdapter();
        lv_devices = (ListView) findViewById(R.id.list_view_devices);
        
        
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
    }
    
    public void btnOnClicked(View v){
        if(!bt_adapter.isEnabled()){
            Intent turn_on_intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turn_on_intent,0);
            Toast.makeText(getApplicationContext(),getString(R.string.bt_turned_on), Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(getApplicationContext(),getString(R.string.bt_already_on), Toast.LENGTH_SHORT).show();
        }
    }
    
    public void btnOffClicked(View v){
        bt_adapter.disable();
        Toast.makeText(getApplicationContext(),getString(R.string.bt_turned_off), Toast.LENGTH_SHORT).show();
    }

    public void btnGetVisibleClicked(View v){
        Intent it_getVisible = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        startActivityForResult(it_getVisible,0);
    }

    public void btnListClicked(View v){
        pairedDevices = bt_adapter.getBondedDevices();

        ArrayList<String> devices_list = new ArrayList<>();

        for(BluetoothDevice bt_dev : pairedDevices) {
            devices_list.add(bt_dev.getName() + " - " + bt_dev.getAddress());
        }
        Toast.makeText(getApplicationContext(), getString(R.string.bt_show_devices), Toast.LENGTH_SHORT).show();

        final ArrayAdapter<String> list_dev_adapter = new ArrayAdapter<>(
                this, android.R.layout.simple_list_item_1, devices_list);

        lv_devices.setAdapter(list_dev_adapter);
        l
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
