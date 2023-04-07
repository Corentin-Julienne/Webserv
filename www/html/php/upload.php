<?php
// Dossier de destination pour les fichiers uploadés
$target_dir = "uploads/";

// Vérifiez si le dossier de destination existe, sinon créez-le
if (!file_exists($target_dir)) {
    mkdir($target_dir, 0777, true);
}

// Chemin du fichier de destination
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);

// Vérifier si le fichier est une véritable image ou non
if (isset($_POST["submit"])) {
    $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
    if ($check !== false) {
        echo "Le fichier est une image - " . $check["mime"] . ".";
        $uploadOk = 1;
    } else {
        echo "Le fichier n'est pas une image.";
        $uploadOk = 0;
    }
}

// Vérifier si le fichier existe déjà
if (file_exists($target_file)) {
    echo "Le fichier existe déjà.";
    $uploadOk = 0;
}

// Vérifier la taille du fichier
if ($_FILES["fileToUpload"]["size"] > 500000) { // Limite de taille de fichier à 500KB
    echo "Désolé, votre fichier est trop volumineux.";
    $uploadOk = 0;
}

// Autoriser certains formats de fichiers
$imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
if ($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg" && $imageFileType != "gif") {
    echo "Désolé, seuls les fichiers JPG, JPEG, PNG et GIF sont autorisés.";
    $uploadOk = 0;
}

// Vérifier si $uploadOk est défini à 0 par une erreur
if ($uploadOk == 0) {
    echo "Désolé, votre fichier n'a pas été uploadé.";
} else {
    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
        echo "Le fichier " . htmlspecialchars(basename($_FILES["fileToUpload"]["name"])) . " a été uploadé avec succès.";
    } else {
        echo "Désolé, une erreur s'est produite lors de l'upload de votre fichier.";
    }
}
?>