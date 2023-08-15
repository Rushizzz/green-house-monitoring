// Import the functions you need from the SDKs you need
import { initializeApp } from 'firebase/app';
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: ".......your credentials.......",
  authDomain: ".......your credentials.......",
  projectId: ".......your credentials.......",
  storageBucket: ".......your credentials.......",
  messagingSenderId: ".......your credentials.......",
  appId: ".......your credentials.......",
  measurementId: ".......your credentials......."
};

// Initialize Firebase
 const firebase = initializeApp(firebaseConfig);

// const db = getFirestore(firebase);

export default firebase;
