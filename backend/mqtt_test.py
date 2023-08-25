import paho.mqtt.client as mqtt
import time

# MQTT broker settings
mqtt_broker_address = "4.213.73.200"
mqtt_broker_port = 1883
mqtt_username = "on_esp32"
mqtt_password = "espesp@90"
mqtt_topic = "test_topic"  # Change this to the topic you want to test

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
    else:
        print("Connection to MQTT broker failed")

        # print why the connection failed
        if rc == mqtt.CONNACK_REFUSED_PROTOCOL_VERSION:
            print("Connection refused: incorrect MQTT protocol version")
        elif rc == mqtt.CONNACK_REFUSED_IDENTIFIER_REJECTED:
            print("Connection refused: invalid client identifier")
        elif rc == mqtt.CONNACK_REFUSED_SERVER_UNAVAILABLE:
            print("Connection refused: server unavailable")
        elif rc == mqtt.CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
            print("Connection refused: incorrect username or password")
        elif rc == mqtt.CONNACK_REFUSED_NOT_AUTHORIZED:
            print("Connection refused: not authorized")
        else:
            print("Connection refused: unknown reason")

def on_publish(client, userdata, mid):
    print("Message published")

def main():
    client = mqtt.Client()
    client.username_pw_set(mqtt_username, mqtt_password)
    client.on_connect = on_connect
    client.on_publish = on_publish

    print("Connecting to MQTT broker...")
    client.connect(mqtt_broker_address, mqtt_broker_port)
    client.loop_start()

    time.sleep(1)  # Give some time for the connection to establish

    test_message = "Hello, MQTT!"
    print(f"Publishing message: {test_message}")
    result = client.publish(mqtt_topic, test_message)

    if result.rc == mqtt.MQTT_ERR_SUCCESS:
        print("Message sent successfully")
    else:
        print("Failed to send message")

    client.loop_stop()
    client.disconnect()

if __name__ == "__main__":
    main()
