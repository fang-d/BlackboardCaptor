package net.fangd.bbc

import android.content.Context
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.media.MediaScannerConnection
import android.net.Uri
import android.os.Build
import android.provider.MediaStore
import android.widget.Toast
import androidx.core.content.ContextCompat
import java.io.File


const val CUT_IMAGE = 1
const val SHARPENED_IMAGE = 2
const val BINARIZED_IMAGE = 4

fun saveToAlbumByPath(context: Context, imagePath: String) {
    val sdk29AndUp = Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q
    if (sdk29AndUp || checkPermission(context, android.Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
        val resultPath = MediaStore.Images.Media.insertImage(
            context.contentResolver, imagePath,
            "BlackboardImage_${System.currentTimeMillis()}.jpg", null
        )
        MediaScannerConnection.scanFile(context, arrayOf(resultPath), null, null)
    }
}

fun saveToAlbumByFile(context: Context, imageFile: File) {
    saveToAlbumByPath(context, imageFile.absolutePath)
}

fun shortToast(context: Context, message: String?) {
    if (message != null)
        Toast.makeText(context, message, Toast.LENGTH_SHORT).show()
}

fun getBitmapFromUri(context: Context, imageUri: Uri) : Bitmap {
    return BitmapFactory.decodeStream(context.contentResolver.openInputStream(imageUri))
}

fun checkPermission(context: Context, permission: String) : Boolean {
    if (ContextCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
        noPermission(context, permission)
        return false
    }
    return true
}

fun noPermission(context: Context, permission: String) {
    var i = permission.length
    while (--i != 0) {
        if (permission[i] == '.')
            break
    }
    shortToast(context, context.getString(R.string.permission_denial) + ": " + permission.substring(i + 1))
}
