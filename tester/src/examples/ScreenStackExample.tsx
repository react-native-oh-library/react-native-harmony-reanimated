import * as React from 'react';
import {Button, StyleSheet, View} from 'react-native';
import {NavigationContainer, useNavigation} from '@react-navigation/native';
import Animated, {
  Easing,
  useAnimatedStyle,
  useSharedValue,
  withRepeat,
  withTiming,
} from 'react-native-reanimated';
import {
  StackNavigationProp,
  createStackNavigator,
} from '@react-navigation/stack';

type RootStackParamList = {
  First: undefined;
  Second: undefined;
};

type FirstScreenProp = StackNavigationProp<RootStackParamList, 'First'>;

const Stack = createStackNavigator<RootStackParamList>();

function FirstScreen() {
  const navigation = useNavigation<FirstScreenProp>();

  const handlePress = () => {
    navigation.navigate('Second');
  };

  return (
    <View style={styles.container}>
      <Button onPress={handlePress} title="Go to next screen" />
    </View>
  );
}

function SecondScreen() {
  const sv = useSharedValue(0);

  React.useEffect(() => {
    sv.value = withRepeat(
      withTiming(2 * Math.PI, {easing: Easing.linear, duration: 1000}),
      -1,
    );
    return () => {
      sv.value = 0;
    };
  });

  const animatedStyle = useAnimatedStyle(() => {
    return {width: (1 + Math.sin(sv.value)) * 150};
  }, [sv]);

  return (
    <View style={styles.container}>
      <Animated.View style={[styles.box, animatedStyle]} />
    </View>
  );
}

export default function ScreenStackExample() {
  return (
    <NavigationContainer independent={true}>
      <Stack.Navigator>
        <Stack.Screen name="First" component={FirstScreen} />
        <Stack.Screen name="Second" component={SecondScreen} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    backgroundColor: 'navy',
    height: 100,
  },
});
