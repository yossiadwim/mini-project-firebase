package com.example.iotapp

import android.content.ContentValues.TAG
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.ValueEventListener
import com.google.firebase.database.ktx.database
import com.google.firebase.ktx.Firebase

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val textValuejarak = findViewById<TextView>(R.id.valueJarak)
        val textStatus = findViewById<TextView>(R.id.status)


//        val dataRef = Firebase.database.getReference("/esp32/")

        val database = Firebase.database
        val dataRef = database.getReference("/esp32/")
        dataRef.addValueEventListener( object : ValueEventListener{

            override fun onDataChange(snapshot: DataSnapshot) {

                var valueldr = snapshot.child("/jarak").getValue().toString()
                textValuejarak.setText(valueldr)



                var valueled = snapshot.child("/status").getValue().toString()
                textStatus.setText(valueled)
            }

            override fun onCancelled(error: DatabaseError) {
                Log.w(TAG, "Failed to read value.", error.toException())
            }

        })
}
}