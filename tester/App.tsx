import './src/types';

import {
  ActivityIndicator,
  FlatList,
  Platform,
  Pressable,
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  View,
} from 'react-native';
import {GestureHandlerRootView} from 'react-native-gesture-handler';
import {
  HeaderBackButton,
  HeaderBackButtonProps,
} from '@react-navigation/elements';
import {
  NavigationContainer,
  NavigationProp,
  PathConfigMap,
  useNavigation,
} from '@react-navigation/native';
import {
  StackNavigationProp,
  createStackNavigator,
} from '@react-navigation/stack';

import {EXAMPLES} from './src/examples';
import React from 'react';
import {useReducedMotion} from 'react-native-reanimated';

import {enableScreens} from 'react-native-screens';

enableScreens(false);

type RootStackParamList = {[P in keyof typeof EXAMPLES]: undefined} & {
  Home: undefined;
};

interface HomeScreenProps {
  navigation: StackNavigationProp<RootStackParamList, 'Home'>;
}

const EXAMPLES_NAMES = Object.keys(EXAMPLES) as (keyof typeof EXAMPLES)[];

function findExamples(search: string) {
  if (search === '') {
    return EXAMPLES_NAMES;
  }
  return EXAMPLES_NAMES.filter(name =>
    EXAMPLES[name].title
      .toLocaleLowerCase()
      .includes(search.toLocaleLowerCase()),
  );
}

function HomeScreen({navigation}: HomeScreenProps) {
  const [search, setSearch] = React.useState('');

  React.useLayoutEffect(() => {
    navigation.setOptions({
      headerSearchBarOptions: {
        onChangeText: event => {
          setSearch(event.nativeEvent.text);
        },
        onSearchButtonPress: event => {
          const results = findExamples(event.nativeEvent.text);
          if (results.length >= 1) {
            navigation.navigate(results[0]);
          }
        },
      },
      headerTransparent: false,
    });
  }, [navigation]);

  return (
    <FlatList
      data={findExamples(search)}
      initialNumToRender={EXAMPLES_NAMES.length}
      renderItem={({item: name}) => (
        <Item
          icon={EXAMPLES[name].icon}
          title={EXAMPLES[name].title}
          onPress={() => navigation.navigate(name)}
        />
      )}
      renderScrollComponent={props => <ScrollView {...props} />}
      ItemSeparatorComponent={ItemSeparator}
      style={styles.list}
    />
  );
}

interface ItemProps {
  icon?: string;
  title: string;
  onPress: () => void;
}

function Item({icon, title, onPress}: ItemProps) {
  return (
    <Pressable style={styles.button} onPress={onPress}>
      {icon && <Text style={styles.title}>{icon + '  '}</Text>}
      <Text style={styles.title}>{title}</Text>
    </Pressable>
  );
}

function ItemSeparator() {
  return <View style={styles.separator} />;
}

const Stack = createStackNavigator<RootStackParamList>();

const linking = {
  prefixes: [],
  config: {
    screens: EXAMPLES_NAMES.reduce<PathConfigMap<RootStackParamList>>(
      (acc, name) => {
        acc[name] = name;
        return acc;
      },
      {Home: ''},
    ),
  },
};

function BackButton(props: HeaderBackButtonProps) {
  const navigation = useNavigation<NavigationProp<RootStackParamList>>();

  return (
    <HeaderBackButton {...props} onPress={() => navigation.navigate('Home')} />
  );
}

export default function App() {
  const [isReady, setIsReady] = React.useState(__DEV__ ? false : true);

  React.useEffect(() => {
    const restoreState = async () => {
      setIsReady(true);
    };

    if (!isReady) {
      restoreState();
    }
  }, [isReady]);

  const shouldReduceMotion = useReducedMotion();

  if (!isReady) {
    return (
      <View style={[styles.container, styles.center]}>
        <ActivityIndicator />
      </View>
    );
  }

  return (
    <View style={styles.container}>
      <StatusBar />
      <SafeAreaView>
        <GestureHandlerRootView style={styles.container}>
          <NavigationContainer linking={linking}>
            <Stack.Navigator>
              <Stack.Screen
                name="Home"
                component={HomeScreen}
                options={{
                  headerTitle: '🐎 Reanimated examples',
                  title: 'Reanimated examples',
                  headerLeft: Platform.OS === 'web' ? () => null : undefined,
                }}
              />
              {EXAMPLES_NAMES.map(name => (
                <Stack.Screen
                  key={name}
                  name={name}
                  component={EXAMPLES[name].screen}
                  options={{
                    animation: shouldReduceMotion ? 'fade' : 'default',
                    headerTitle: EXAMPLES[name].title,
                    title: EXAMPLES[name].title,
                    headerLeft: Platform.OS === 'web' ? BackButton : undefined,
                  }}
                />
              ))}
            </Stack.Navigator>
          </NavigationContainer>
        </GestureHandlerRootView>
      </SafeAreaView>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  center: {
    alignItems: 'center',
    justifyContent: 'center',
  },
  list: {
    backgroundColor: '#EFEFF4',
  },
  separator: {
    height: 1,
    backgroundColor: '#DBDBE0',
  },
  button: {
    flex: 1,
    height: 60,
    padding: 15,
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: 'white',
  },
  title: {
    fontSize: 16,
    color: 'black',
  },
});
