package ufu.biolab.italo.idbracelet;

import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;

public class CadastroActivity extends AppCompatActivity {

    Button btnOk, btnProblem;
    EditText editTextName, editTextBirth, editTextPhone, editTextEmail;
    RadioGroup radioGroupSex;
    RadioButton radioButtonMale, radioButtonFemale;
    String sex_selected;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cadastro);
        btnOk = (Button) findViewById(R.id.buttonOk);
        btnProblem = (Button) findViewById(R.id.buttonProblem);
        editTextName = (EditText) findViewById(R.id.editTextName);
        editTextBirth = (EditText) findViewById(R.id.editTextBirth);
        editTextPhone = (EditText) findViewById(R.id.editTextPhone);
        editTextEmail = (EditText) findViewById(R.id.editTextEmail);
        radioButtonMale = (RadioButton) findViewById(R.id.radioButtonMale);
        radioButtonFemale = (RadioButton) findViewById(R.id.radioButtonFemale);
        sex_selected = "Nao Marcado";
        radioButtonMale.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sex_selected = "Male";
                Toast.makeText(CadastroActivity.this, "Selected sex: Male", Toast.LENGTH_SHORT).show();
            }
        });
        radioButtonFemale.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sex_selected = "Female";
                Toast.makeText(CadastroActivity.this, "Selected sex: Female", Toast.LENGTH_SHORT).show();
            }
        });
        btnOk.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(CadastroActivity.this,
                        "Your name: " +
                        editTextName.getText().toString() +
                        "\nNascimento: " +
                        editTextBirth.getText().toString() +
                        "\nTelefone: " +
                        editTextPhone.getText().toString() +
                        "\nEmail: " +
                        editTextEmail.getText().toString() +
                        "\nSexo: " +
                        sex_selected,
                        Toast.LENGTH_LONG).show();
            }
        });
        btnProblem.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(CadastroActivity.this, "Procure os desenvolvedores.", Toast.LENGTH_SHORT).show();
            }
        });
    }
}
