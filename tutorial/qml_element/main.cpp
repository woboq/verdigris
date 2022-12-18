#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
  auto app = QGuiApplication{argc, argv};
  auto engine = QQmlApplicationEngine{};
  const auto url = QUrl(u"qrc:/QmlTutorial/main.qml"_qs);
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
      &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      }, Qt::QueuedConnection);
  engine.load(url);
  return app.exec();
}
