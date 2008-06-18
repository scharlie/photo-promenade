<?php
/**
 *  Helper functions, mainly from Drupal
 *  http://drupal.org
 */
 
function scale($source, $target, $size) {

   // Dissect filename to find dimension information
   if (preg_match('#^([0-9]+)x?$#i', $size, $match)) {
       // Resize to width: 600 or 600x
       $width = (int)$match[1];
       $height = -1;
   } else if (preg_match('#^x([0-9]+)$#i', $size, $match)) {
       // Resize to height: x480
       $width = -1;
       $height = (int)$match[1];
   } else if (preg_match('#^([0-9]+)x([0-9]+)$#i', $size, $match)) {
       // Resize to fit: 600x480
       $width = (int)$match[1];
       $height = (int)$match[2];
   } else {
       // no resizing, fail silently
       return FALSE;
   }

   if (gd_available()) {
       image_scale($source, $target, $width, $height);
   }
}

function image_scale($source, $destination, $width, $height) {
  $info = image_get_info($source);

  // Don't scale up
  if ($width > $info['width'] && $height > $info['height']) {
    return false;
  }

  $aspect = $info['height'] / $info['width'];
  if ($height < 0 || $aspect < $height / $width) {
    $width = (int)min($width, $info['width']);
    $height = (int)round($width * $aspect);
  } else {
    $height = (int)min($height, $info['height']);
    $width = (int)round($height / $aspect);
  }

  return image_gd_resize($source, $destination, $width, $height);
}

/**
  * GD2 has to be available on the system
  *
  * @return boolean
  */
function gd_available() {
  if ($check = get_extension_funcs('gd')) {
    if (in_array('imagegd2', $check)) {
      // GD2 support is available.
      return true;
    }
  }
  return false;
}

/**
  * Get details about an image.
  *
  * @return array containing information about the image
  *      'width': image's width in pixels
  *      'height': image's height in pixels
  *      'extension': commonly used extension for the image
  *      'mime_type': image's MIME type ('image/jpeg', 'image/gif', etc.)
  */
function image_get_info($file) {
  if (!is_file($file)) {
    return false;
  }

  $details = false;
  $data = @getimagesize($file);

  if (is_array($data)) {
    $extensions = array('1' => 'gif', '2' => 'jpg', '3' => 'png');
    $extension = array_key_exists($data[2], $extensions) ?  $extensions[$data[2]] : '';
    $details = array('width'     => $data[0],
      'height'    => $data[1],
      'extension' => $extension,
      'mime_type' => $data['mime']);
  }

  return $details;
}

/**
  * Scale an image to the specified size using GD.
  */
function image_gd_resize($source, $destination, $width, $height) {
  if (!file_exists($source)) {
    return false;
  }

  $info = image_get_info($source);
  if (!$info) {
    return false;
  }

  $im = image_gd_open($source, $info['extension']);
  if (!$im) {
    return false;
  }


  $res = imageCreateTrueColor($width, $height);
  imageCopyResampled($res, $im, 0, 0, 0, 0, $width, $height, $info['width'], $info['height']);
  $result = image_gd_close($res, $destination, $info['extension']);

  imageDestroy($res);
  imageDestroy($im);

  return $result;
}


/**
  * GD helper function to create an image resource from a file.
  */
function image_gd_open($file, $extension) {
  $extension = str_replace('jpg', 'jpeg', $extension);
  $open_func = 'imagecreatefrom'. $extension;
  if (!function_exists($open_func)) {
    return false;
  }
  return $open_func($file);
}

/**
  * GD helper to write an image resource to a destination file.
  */
function image_gd_close($res, $destination, $extension) {
  $extension = str_replace('jpg', 'jpeg', $extension);
  $close_func = 'image'. $extension;
  if (!function_exists($close_func)) {
    return false;
  }
  return $close_func($res, $destination);
}


?>