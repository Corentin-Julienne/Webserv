<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>test</title>
</head>
<body>


<?php
echo "POST :" ;
  foreach ($_POST as $key => $value) 
  {
        echo $key;
        echo " :";
        echo $value;
        echo "<br>";
  }
?>
<?php
  echo "GET : \n";
  foreach ($_GET as $key => $value) 
  {
        echo $key;
        echo " :";
        echo $value;
        echo "<br>";
  }
?>
</body>
</html>
