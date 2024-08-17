import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick3D 1.15
import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls 2.15




    View3D {

             property double roll: 0
             property double yaw: 0
             property double pitch: 0

        id: view
        anchors.fill: parent
        camera: camera
        renderMode: View3D.Overlay


        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 0, 350)

            eulerRotation.x: 0
        }

        DirectionalLight {
                    eulerRotation.x: 10
                }



        Model {

            id: cylinder
            visible: true
            position: Qt.vector3d(0,0,0)

            source: "#Cylinder"


          materials: DefaultMaterial {
          diffuseColor:  "#d9b99b"
          specularAmount: 4
          specularRoughness: 4
          diffuseMap: Texture{source: "qrc:/img/img/mesh2.jpg"}


                   }




          scale.x: 1.5
          scale.y: 2.5
          scale.z: 1.5

        eulerRotation.x: pitch
        eulerRotation.y: roll
        eulerRotation.z: yaw



                     }

    /*    // Wireframe Model
         Model {
             visible: true
             position: Qt.vector3d(0,0,0)
             source: "#Cylinder"

             materials: DefaultMaterial {
                 diffuseColor: "#FFFF00"
                 lighting: DefaultMaterial.NoLighting
                 opacity: 0.6
             }

             scale.x: 1.56
             scale.y: 2.56
             scale.z: 1.56

             eulerRotation.x: pitch
             eulerRotation.y: roll
             eulerRotation.z: yaw
         }
*/

Timer {
                                 id: updateTimer
                                 interval: 1000// Update every 1000 milliseconds (1 second)
                                 repeat: true
                                 running: true

                                 onTriggered: {


                                    eulerFunction(roll, yaw, pitch)
                                    //   console.log(roll, yaw, pitch)

}





}

            function eulerFunction(rolll, yaww, pitchh){
    cylinder.eulerRotationChanged(roll - rolll, yaw - yaww, pitch - pitchh)
                roll = rolll
                yaw = yaww
                pitch = pitchh





            }


            Rectangle {

                anchors.fill: parent

                Image {
                    anchors.centerIn: backgroundImage
                    source: "qrc:/img/img/durus.png"

                }

                Text {
                    id: text
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    color: "#ffffff"
                    font.pixelSize: 20
                    text: qsTr(" 3D DURUŞ BİLGİSİ")
                    font.family: "Arial"
                    font.bold: true
                    style: Text.Outline
                }
                Text {
                        id: textt

                        anchors.centerIn: parent.Center
                        color: "#ffffff"
                        font.pixelSize: 15
                        text: qsTr(" PITCH   ROLL   YAW")
                        font.family: "Arial"
                        font.bold: true
                        style: Text.Outline
                }
    }







        }
