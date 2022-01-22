let socket = WebSocket.prototype // 

const command = {
    state: () => ({
        batteryVoltage: 0

    }),
    mutations: {
        setBatteryVoltage: function (state, val) {
            state.batteryVoltage = val
        }
    },
    actions: {
        initCommand: function (context) {
            // socket = new WebSocket('ws://' + '192.168.2.103' + ':81');
            socket = new WebSocket('ws://'+window.location.host+':81');
            socket.addEventListener('message', function (event) {
                let data = JSON.parse(event.data)
                if (data.batteryVoltage)
                    context.commit('setBatteryVoltage', data.batteryVoltage)
            });
        },


    },
    getters: {
        getBatteryVoltage: function (state) {
            return state.batteryVoltage
        }
    }
}
export default command
