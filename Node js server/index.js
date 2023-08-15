import express from "express"
import firebase from "./firebaseConfig.js"
import { collection, addDoc, getFirestore } from "firebase/firestore";
import { async } from "@firebase/util";

let app = express(); 
var PORT = process.env.PORT;
  
// For parsing application/json
app.use(express.json());
  
// For parsing application/x-www-form-urlencoded
app.use(express.urlencoded({ extended: true }));
  
app.listen(PORT).setTimeout(500);
app.post('/myPost', function (req, res) {
  
  const db = getFirestore(firebase);
  
  console.log(req.body);
  let data = req.body;``
  
  void async function() {
    try {
      const docRef = await addDoc(collection(db, "users"), data);
      console.log("Document written with ID: ", docRef.id);
    } catch (e) {
      console.error("Error adding document: ", e);
    }
  }();

  res.send();
});