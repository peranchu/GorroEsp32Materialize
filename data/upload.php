<?php

$targetPatch = "SD/" .basename($_FILES["datos"]["name"]);
move_uploaded_file($_FILES["datos"]["tmp_name"], $targetPatch);