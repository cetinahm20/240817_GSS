import QtQuick 2.0
import QtLocation 5.15
import QtPositioning 5.11

Rectangle {


    id:window

    property double lattitude: 0
    property double longitude: 0

    property Component locationmarker: locmarker

    Plugin{

        id:googlemapview
        name:"osm"
    }
    Map{


        id:mapview
        anchors.fill: parent
        plugin:googlemapview
        center:QtPositioning.coordinate(lattitude,longitude)
        zoomLevel: 12
        activeMapType: supportedMapTypes[3]
    }


    Timer {
            id: updateTimer
            interval: 1000 // Update every 1000 milliseconds (1 second)
            repeat: true
            running: true

            onTriggered: {
                // Simulate changing latitude and longitude
                    setCenter(lattitude,longitude)
                    setLocMarker(lattitude,longitude)

            }
    }
    function setCenter(lati, longi)
      {

        mapview.pan(lattitude - lati, longitude - longi)
        lattitude = lati
        longitude = longi
    }
    function setLocMarker(lati, longi)
      {
        var item = locmarker.createObject(window,{
                                          coordinate:QtPositioning.coordinate(lati,longi)
                                          })
        mapview.addMapItem(item)
    }
    Component{
    id:locmarker
    MapQuickItem{

    id:markering
    anchorPoint.x: image.width/4
    anchorPoint.y: image.height
    coordinate: position
    sourceItem: Image {
        id:image
        width:20
        height:20
        source: "qrc:/img/img/location-pin.png"
    }

    }

    }


}






