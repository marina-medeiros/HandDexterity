import json
import re
import threading
import time

import matplotlib.pyplot as plt
import paho.mqtt.client as mqtt
import streamlit as st

st.set_page_config(page_title="HandDexterity Dashboard", layout="wide")

TARGET_X, TARGET_Y, TARGET_W, TARGET_H = 39, 14, 50, 32
RECTANGLE_EXPECTED = [
    (TARGET_X, TARGET_Y),                       
    (TARGET_X + TARGET_W, TARGET_Y),            
    (TARGET_X + TARGET_W, TARGET_Y + TARGET_H), 
    (TARGET_X, TARGET_Y + TARGET_H),           
    (TARGET_X, TARGET_Y),                      
]

TRIANGLE_EXPECTED = [
    (64, 20),
    (88, 46),
    (40, 46), 
    (64, 20),
]

EXPECTED_SHAPES = {
    "rectangle": RECTANGLE_EXPECTED,
    "triangle": TRIANGLE_EXPECTED,
}

TOPIC_PATTERN = re.compile(r".*/test(\d+)/(rectangle|triangle)/(data|trajectory)$")
SENSIBILITY_TOPIC_PATTERN = re.compile(r".*/test(\d+)(?:/sensibility)?$")


def parse_topic(topic):
    match = TOPIC_PATTERN.match(topic)
    if not match:
        return None
    test_id, shape, kind = match.groups()
    return int(test_id), shape, kind


def parse_sensibility(topic, payload):
    match = SENSIBILITY_TOPIC_PATTERN.match(topic)
    if not match:
        return None
    test_id = int(match.group(1))

    text = payload.strip()
    if text.startswith("sensibility:"):
        text = text[len("sensibility:"):]

    try:
        value = float(text)
    except ValueError:
        return None

    return test_id, value


def make_mqtt_client(broker_ip, broker_port, base_topic, store, lock):
    def on_connect(client, userdata, flags, reason_code, properties=None):
        client.subscribe(base_topic)

    def on_message(client, userdata, msg):
        payload = msg.payload.decode("utf-8", errors="replace")

        parsed = parse_topic(msg.topic)
        if parsed is not None:
            test_id, shape, kind = parsed
            try:
                value = json.loads(payload)
            except json.JSONDecodeError:
                return
            with lock:
                store.setdefault(test_id, {}).setdefault(shape, {})[kind] = value
            return

        sens = parse_sensibility(msg.topic, payload)
        if sens is not None:
            test_id, value = sens
            with lock:
                store.setdefault(test_id, {})["sensibility"] = value

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(broker_ip, broker_port, keepalive=60)
    return client


if "results" not in st.session_state:
    st.session_state.results = {}
if "lock" not in st.session_state:
    st.session_state.lock = threading.Lock()
if "mqtt_started" not in st.session_state:
    st.session_state.mqtt_started = False

st.sidebar.header("Configuração MQTT")
broker_ip = st.sidebar.text_input("IP do broker", value="54.36.178.49")
broker_port = st.sidebar.number_input("Porta", value=1883, step=1)
base_topic = st.sidebar.text_input("Tópico base", value="UFRN/HandDexterity/#")
auto_refresh = st.sidebar.checkbox("Atualização automática (2s)", value=True)

if st.sidebar.button("Conectar") or not st.session_state.mqtt_started:
    if not st.session_state.mqtt_started:
        client = make_mqtt_client(
            broker_ip, int(broker_port), base_topic,
            st.session_state.results, st.session_state.lock,
        )
        thread = threading.Thread(target=client.loop_forever, daemon=True)
        thread.start()
        st.session_state.mqtt_started = True
        st.sidebar.success(f"Conectado a {broker_ip}:{broker_port}")

st.title("HandDexterity — Resultados dos Testes")

with st.session_state.lock:
    results_snapshot = {k: v for k, v in st.session_state.results.items()}

if not results_snapshot:
    st.info("Nenhum dado recebido ainda. Rode um teste no Pico para ver os resultados aqui.")
else:
    for test_id in sorted(results_snapshot.keys(), reverse=True):
        st.subheader(f"Teste {test_id}")

        sensibility = results_snapshot[test_id].get("sensibility")
        if sensibility is not None:
            st.caption(f"Sensibilidade: {sensibility:.2f}")

        shapes = {k: v for k, v in results_snapshot[test_id].items() if k != "sensibility"}
        cols = st.columns(len(shapes) if shapes else 1)

        for col, shape in zip(cols, sorted(shapes.keys())):
            with col:
                st.markdown(f"**{shape.capitalize()}**")
                data = shapes[shape].get("data")
                trajectory = shapes[shape].get("trajectory")

                if data:
                    m1, m2 = st.columns(2)
                    m1.metric("Accuracy", f"{data.get('accuracy', 0):.1f}%")
                    m2.metric("Score", f"{data.get('score', 0):.1f}")
                    m1.metric("Tempo (ms)", f"{data.get('time_ms', 0)}")
                    m2.metric("Vel. média", f"{data.get('average_speed', 0):.2f}")
                else:
                    st.caption("Sem dados de resultado ainda.")

                if trajectory:
                    fig, ax = plt.subplots(figsize=(1.8, 1.4), dpi=100)

                    expected = EXPECTED_SHAPES.get(shape)
                    if expected:
                        ex = [p[0] for p in expected]
                        ey = [p[1] for p in expected]
                        ax.plot(ex, ey, linestyle="--", color="gray", linewidth=1)

                    xs = [p[0] for p in trajectory]
                    ys = [p[1] for p in trajectory]
                    ax.plot(xs, ys, marker="o", markersize=1, linewidth=0.8, color="tab:blue")

                    ax.invert_yaxis()
                    ax.set_xticks([])
                    ax.set_yticks([])
                    ax.set_aspect("equal")
                    ax.set_title("Trajetória", fontsize=8)
                    ax.legend(fontsize=5, loc="upper right", frameon=False)
                    st.pyplot(fig, use_container_width=False)
                    plt.close(fig)
                else:
                    st.caption("Sem trajetória ainda.")

        st.divider()

if auto_refresh:
    time.sleep(2)
    st.rerun()