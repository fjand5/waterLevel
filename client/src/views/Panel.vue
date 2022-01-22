<template>
  <div class="panel">
    {{getBatteryVoltage}}
  </div>
</template>

<script>
import { mapGetters } from "vuex";
export default {
  data() {
    return {    };
  },
  computed: {
    ...mapGetters([
      "getBatteryVoltage"
    ]),
  },
  mounted: function () {},
  watch: {
  },
  methods: {},
};
</script>

<style>
</style>