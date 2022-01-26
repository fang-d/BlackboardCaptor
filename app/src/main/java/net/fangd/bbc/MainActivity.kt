package net.fangd.bbc

import android.Manifest
import android.graphics.Bitmap
import android.net.Uri
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.widget.Button
import android.widget.ImageView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.FileProvider
import java.io.BufferedInputStream
import java.io.BufferedOutputStream
import java.io.File

class MainActivity : AppCompatActivity() {
    companion object {
        init {
            System.loadLibrary("opencv_java4")
            System.loadLibrary("BlackboardCaptor")
        }
        private var image : Bitmap? = null                      // Image shown in imageView
    }
    external fun process(src: String?, dst: String?, favour: Int) : Int

    private lateinit var imageView : ImageView              // ImageView to show the image
    private lateinit var sourceFile : File                  // File of source image in externalCacheDir
    private lateinit var targetFile : File                  // File of target image

    private lateinit var sourceUri : Uri                    // Uri that points to the selected image
    private lateinit var targetUri : Uri                    // Uri that points to the processed image
    private var imageMode = CUT_IMAGE or SHARPENED_IMAGE    // Style of result image

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        this.imageView = findViewById(R.id.imageView)
        this.sourceFile = File(externalCacheDir, getString(R.string.source_image))
        this.sourceUri = FileProvider.getUriForFile(this, getString(R.string.provider), sourceFile)
        this.targetFile = File(externalCacheDir, getString(R.string.target_image))
        this.targetUri = FileProvider.getUriForFile(this, getString(R.string.provider), targetFile)

        findViewById<Button>(R.id.takePhotoButton).setOnClickListener {
            if (checkPermission(this, Manifest.permission.CAMERA))
                takePhoto.launch(sourceUri)
        }
        findViewById<Button>(R.id.selectPhotoButton).setOnClickListener {
            if (checkPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE))
                selectPhoto.launch(arrayOf("image/*"))
        }

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            askPermissions.launch(arrayOf(
                Manifest.permission.INTERNET,
                Manifest.permission.ACCESS_NETWORK_STATE,
                Manifest.permission.CAMERA,
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            ))
        } else {
            askPermissions.launch(arrayOf(
                Manifest.permission.INTERNET,
                Manifest.permission.ACCESS_NETWORK_STATE,
                Manifest.permission.CAMERA,
                Manifest.permission.READ_EXTERNAL_STORAGE
            ))
        }
        if (image != null)
            this.imageView.setImageBitmap(image)
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        super.onCreateOptionsMenu(menu)
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        this.imageMode = when (item.itemId) {
            R.id.crop_image -> CUT_IMAGE
            R.id.sharpen_image -> CUT_IMAGE or SHARPENED_IMAGE
            R.id.binarize_image -> CUT_IMAGE or SHARPENED_IMAGE or BINARIZED_IMAGE
            else -> throw Exception("Menu out of bound")
        }
        return super.onOptionsItemSelected(item)
    }

    private val takePhoto = registerForActivityResult(
        ActivityResultContracts.TakePicture()) { isSuccess ->
        clearCache()
        if (isSuccess) {
            showImage(this.sourceUri)
            saveToAlbumByFile(this, this.sourceFile)
            processImage()
        } else {
            shortToast(this, getString(R.string.failed2take))
        }
    }

    private val selectPhoto = registerForActivityResult(
        ActivityResultContracts.OpenDocument()) {  imageResultUri ->
        clearCache()
        if (imageResultUri != null) {
            showImage(imageResultUri)
            if (sourceFile.exists())
                sourceFile.delete()

            val sourceImageIS = BufferedInputStream(this.contentResolver.openInputStream(imageResultUri))
            val sourceImageOS = BufferedOutputStream(this.sourceFile.outputStream())
            val buffer = ByteArray(1024)
            var cnt : Int
            while (sourceImageIS.read(buffer).also { cnt = it } != -1) {
                sourceImageOS.write(buffer, 0, cnt)
            }
            sourceImageIS.close()
            sourceImageOS.flush()
            sourceImageOS.close()

            processImage()
        } else {
            shortToast(this, getString(R.string.failed2select))
        }
    }

    private fun processImage() {
        if (process(this.sourceFile.absolutePath, this.targetFile.absolutePath, this.imageMode) != 0) {
            shortToast(this, getString(R.string.no_blackboard))
        } else {
            showImage(this.targetUri)
            saveToAlbumByFile(this, this.targetFile)
        }
    }

    private fun showImage(uri: Uri) {
        image = getBitmapFromUri(this, uri)
        this.imageView.setImageBitmap(image)
    }

    private fun clearCache() {
        if (this.targetFile.exists())
            this.targetFile.delete()
    }

    private val askPermissions =
        registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) {
                permissions : Map<String, Boolean> ->
        permissions.entries.forEach {
            if (!it.value) {
                noPermission(this, it.key)
            }
        }
    }
}
