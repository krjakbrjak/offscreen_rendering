#include <QApplication>
#include <QLabel>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickGraphicsDevice>
#include <QQuickItem>
#include <QQuickRenderControl>
#include <QQuickRenderTarget>
#include <QQuickWindow>
#include <QSurfaceFormat>

int main(int argc, char **argv) {
  QApplication app{argc, argv};

  QSurfaceFormat format;
  format.setMajorVersion(4);
  format.setMinorVersion(6);

  QOpenGLContext gl_ctx;
  gl_ctx.setFormat(format);
  gl_ctx.create();
  if (!gl_ctx.isValid()) {
    return 1;
  }

  QOffscreenSurface surface;
  surface.setFormat(format);
  surface.create();
  if (!surface.isValid()) {
    return 1;
  }

  gl_ctx.makeCurrent(&surface);

  QQmlEngine engine;
  QQmlComponent component{&engine};

  // Create a QQuickWindow that will render the scene to a custom fb
  QQuickRenderControl control;
  QQuickWindow window{&control};
  window.setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(&gl_ctx));

  if (!control.initialize()) {
    qDebug() << "Failed to initialize QQuickRenderControl";
    return 0;
  }

  // Viewport size
  QSize fb_size{400, 300};

  // A custom framebuffer for rendering.
  QOpenGLFramebufferObject fb{fb_size,
                              QOpenGLFramebufferObject::CombinedDepthStencil};

  // Set the custom framebuffer's texture as the render target for the
  // Qt Quick window.
  auto tg = QQuickRenderTarget::fromOpenGLTexture(fb.texture(), fb.size());
  window.setRenderTarget(tg);

  // Label that displays the content of the custom framebuffer.
  QLabel label;
  label.setFixedWidth(fb.size().width());
  label.setFixedHeight(fb.size().height());
  label.show();

  QObject::connect(
      &control, &QQuickRenderControl::sceneChanged, &control,
      [&] {
        control.polishItems();
        control.beginFrame();
        control.sync();
        control.render();
        control.endFrame();

        // To simplify, just display the image.
        label.setPixmap(QPixmap::fromImage(fb.toImage()));
      },
      Qt::QueuedConnection);

  // Reparent the loaded component to the quick window.
  QObject::connect(&component, &QQmlComponent::statusChanged, [&] {
    QObject *rootObject = component.create();
    if (component.isError()) {
      QList<QQmlError> errorList = component.errors();
      foreach (const QQmlError &error, errorList)
        qWarning() << error.url() << error.line() << error;
      return;
    }
    auto rootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!rootItem) {
      qWarning("run: Not a QQuickItem");
      delete rootObject;
      return;
    }

    rootItem->setParentItem(window.contentItem());
  });

  // Load qml.
  component.loadUrl(QUrl::fromLocalFile("main.qml"));

  return app.exec();
}