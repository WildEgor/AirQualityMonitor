package main

import (
	"crypto/rand"
	"fmt"
	"log"
	"math/big"
	"net/url"
	"strconv"
	"sync"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

type MQTTConnectionConfig struct {
	ClientID string
	Server   string
	Port     int
	Username string
	Password string
}

var (
	mqttConfig = MQTTConnectionConfig{
		ClientID: "mqtt_tester",
		Server:   "m8.wqtt.ru",
		Port:     20336,
		Username: "",
		Password: "",
	}

	co2Topic  = "common/aqm/co2"
	tvocTopic = "common/aqm/tvoc"
)

func connect(clientId string, uri *url.URL) mqtt.Client {
	opts := createClientOptions(clientId, uri)
	client := mqtt.NewClient(opts)
	token := client.Connect()

	fmt.Println("try establish connection to MQTT...")

	for !token.WaitTimeout(3 * time.Second) {
	}

	if err := token.Error(); err != nil {
		log.Fatal(err)
	}

	fmt.Println("successfully connected to MQTT!")
	return client
}

func createClientOptions(clientId string, uri *url.URL) *mqtt.ClientOptions {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(fmt.Sprintf("tcp://%s", uri.Host))
	opts.SetUsername(uri.User.Username())
	password, _ := uri.User.Password()
	opts.SetPassword(password)
	opts.SetClientID(clientId)
	return opts
}

func listen(uri *url.URL, topic string) {
	client := connect(fmt.Sprintf("%s_sub", mqttConfig.ClientID), uri)

	client.Subscribe(topic, 0, func(client mqtt.Client, msg mqtt.Message) {
		fmt.Printf("* [%s] %s\n", msg.Topic(), string(msg.Payload()))
	})
}

func main() {
	connectionString := fmt.Sprintf("mqtt://%s:%s@%s:%d",
		mqttConfig.Username,
		mqttConfig.Password,
		mqttConfig.Server,
		mqttConfig.Port,
	)

	uri, err := url.Parse(connectionString)
	if err != nil {
		log.Fatal(err)
	}

	topics := []string{
		co2Topic,
		tvocTopic,
	}

	wg := sync.WaitGroup{}
	for _, topic := range topics {
		wg.Add(1)

		go func() {
			defer wg.Done()
			listen(uri, topic)
		}()
	}

	wg.Wait()

	client := connect(fmt.Sprintf("%s_pub", mqttConfig.ClientID), uri)
	timer := time.NewTicker(5 * time.Second)

	for {
		select {
		case <-timer.C:
			value := GetRandFloat(400, 1500)
			client.Publish(co2Topic, 0, false, strconv.FormatFloat(value, 'f', 6, 64))
			value = GetRandFloat(0, 250)
			client.Publish(tvocTopic, 0, false, strconv.FormatFloat(value, 'f', 6, 64))
		default:
		}
	}
}

const floatPrecision = 100

func GetRandInt(min, max int) int {
	nBig, _ := rand.Int(rand.Reader, big.NewInt(int64(max+1-min)))
	n := nBig.Int64()
	return int(n) + min
}

func GetRandFloat(min, max float64) float64 {
	minInt := int(min * floatPrecision)
	maxInt := int(max * floatPrecision)

	return float64(GetRandInt(minInt, maxInt)) / floatPrecision
}
