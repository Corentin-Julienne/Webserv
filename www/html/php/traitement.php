<?php
header('Content-Type: text/html; charset=utf-8');

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $nom = isset($_POST['nom']) ? htmlspecialchars($_POST['nom']) : '';
    $prenom = isset($_POST['prenom']) ? htmlspecialchars($_POST['prenom']) : '';

    echo "<h1>Bonjour, $prenom $nom !</h1>";
    echo "<p>Voici les données que vous avez soumises :</p>";
    echo "<ul>";
    echo "<li>Nom : $nom</li>";
    echo "<li>Prénom : $prenom</li>";
    echo "</ul>";
} else {
    header('Location: index.html');
    exit();
}
?>
